#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QFlags>
#include <QIODevice>
#include <QJsonDocument>
#include <QMessageBox>
#include <core/document/DocumentBackups.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/presenter/Presenter.hpp>
#include <core/view/View.hpp>
#include <iscore/plugins/application/GUIApplicationPlugin.hpp>
#include <iscore/plugins/panel/PanelDelegate.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <iscore/tools/std/Optional.hpp>

#include <QSaveFile>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <utility>

#include "DocumentManager.hpp"
#include "QRecentFilesMenu.h"
#include <ossia/detail/algorithms.hpp>
#include <core/application/ApplicationSettings.hpp>
#include <core/command/CommandStack.hpp>
#include <core/command/CommandStackSerialization.hpp>
#include <core/document/Document.hpp>
#include <iscore/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <iscore/application/ApplicationComponents.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <iscore/model/Identifier.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
namespace iscore
{
struct LoadedPluginVersions
{
  UuidKey<iscore::Plugin> plugin;
  iscore::Version version;
};
}

namespace bmi = boost::multi_index;
using LocalPluginVersionsMap = bmi::
    multi_index_container<iscore::Plugin_QtInterface*, bmi::indexed_by<bmi::hashed_unique<bmi::const_mem_fun<iscore::Plugin_QtInterface, UuidKey<iscore::Plugin>, &iscore::Plugin_QtInterface::key>>>>;
using LoadedPluginVersionsMap = bmi::
    multi_index_container<iscore::Plugin_QtInterface*, bmi::indexed_by<bmi::hashed_unique<bmi::member<iscore::LoadedPluginVersions, UuidKey<iscore::Plugin>, &iscore::LoadedPluginVersions::plugin>>>>;

namespace std
{
template <>
struct hash<iscore::LoadedPluginVersions>
{
  std::size_t operator()(const iscore::LoadedPluginVersions& kagi) const
      noexcept
  {
    return std::hash<UuidKey<iscore::Plugin>>{}(kagi.plugin);
  }
};
}

namespace iscore
{
DocumentManager::DocumentManager(iscore::View& view, QObject* parentPresenter)
    : m_view{view}, m_builder{parentPresenter, &view}
{
}

void DocumentManager::init(const iscore::GUIApplicationContext& ctx)
{
  con(m_view, &View::activeDocumentChanged, this,
      [&](const Id<DocumentModel>& doc) {
        prepareNewDocument(ctx);
        auto it = ossia::find_if(m_documents, [&](auto other) {
          return other->model().id() == doc;
        });
        setCurrentDocument(ctx, it != m_documents.end() ? *it : nullptr);
      },
      Qt::QueuedConnection);

  con(m_view, &View::closeRequested, this, [&](const Id<DocumentModel>& doc) {
    auto it = ossia::find_if(
        m_documents, [&](auto other) { return other->model().id() == doc; });
    ISCORE_ASSERT(it != m_documents.end());
    closeDocument(ctx, **it);
  });

  m_recentFiles = new QRecentFilesMenu{tr("Recent files"), nullptr};

  QSettings settings("OSSIA", "i-score");
  m_recentFiles->restoreState(settings.value("RecentFiles").toByteArray());

  connect(
      m_recentFiles, &QRecentFilesMenu::recentFileTriggered, this,
      [&](const QString& f) { loadFile(ctx, f); });
}

DocumentManager::~DocumentManager()
{
  saveRecentFilesState();

  // The documents have to be deleted before the application context plug-ins.
  // This is because the Local device has to be deleted last in
  // ApplicationPlugin.
  for (auto document : m_documents)
  {
    document->deleteLater();
  }

  m_documents.clear();
  m_currentDocument = nullptr;
  if (m_recentFiles)
    delete m_recentFiles;
}

ISCORE_LIB_BASE_EXPORT
Document* DocumentManager::setupDocument(
    const iscore::GUIApplicationContext& ctx, Document* doc)
{
  if (doc)
  {
    auto it = ossia::find(m_documents, doc);
    if (it == m_documents.end())
      m_documents.push_back(doc);

    m_view.addDocumentView(&doc->view());
    setCurrentDocument(ctx, doc);
    connect(
        &doc->metadata(), &DocumentMetadata::fileNameChanged, this,
        [=](const QString& s) { m_view.on_fileNameChanged(&doc->view(), s); });
  }
  else
  {
    setCurrentDocument(
        ctx, m_documents.empty() ? nullptr : m_documents.front());
  }

  return doc;
}



void DocumentManager::setCurrentDocument(
    const iscore::GUIApplicationContext& ctx, Document* doc)
{
  auto old = m_currentDocument;
  m_currentDocument = doc;

  if (doc)
  {
    for (auto& panel : ctx.panels())
    {
      panel.setModel(doc->context());
    }
  }
  else
  {
    for (auto& panel : ctx.panels())
    {
      panel.setModel(ossia::none);
    }
  }

  for (auto& ctrl : ctx.guiApplicationPlugins())
  {
    ctrl->on_documentChanged(old, m_currentDocument);
  }
  emit documentChanged(m_currentDocument);
}

bool DocumentManager::closeDocument(
    const iscore::GUIApplicationContext& ctx, Document& doc)
{
  // Warn the user if he might loose data
  if (!doc.commandStack().isAtSavedIndex())
  {
    QMessageBox msgBox;
    msgBox.setText(tr("The document has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret)
    {
      case QMessageBox::Save:
        if (saveDocument(doc))
          break;
        else
          return false;
      case QMessageBox::Discard:
        // Do nothing
        break;
      case QMessageBox::Cancel:
        return false;
        break;
      default:
        break;
    }
  }

  // Close operation
  forceCloseDocument(ctx, doc);
  return true;
}

void DocumentManager::forceCloseDocument(
    const iscore::GUIApplicationContext& ctx, Document& doc)
{
  for(auto plug : doc.model().pluginModels())
  {
    plug->on_documentClosing();
  }

  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

  m_view.closeDocument(&doc.view());
  ossia::remove_one(m_documents, &doc);
  setCurrentDocument(ctx, !m_documents.empty() ? m_documents.back() : nullptr);

  delete &doc;
}

bool DocumentManager::saveDocument(Document& doc)
{
  auto savename = doc.metadata().fileName();

  if (savename.indexOf(tr("Untitled")) == 0)
  {
    saveDocumentAs(doc);
  }
  else if (savename.size() != 0)
  {
    QSaveFile f{savename};
    f.open(QIODevice::WriteOnly);
    if (savename.indexOf(".scorebin") != -1)
      f.write(doc.saveAsByteArray());
    else
    {
      QJsonDocument json_doc;
      json_doc.setObject(doc.saveAsJson());

      f.write(json_doc.toJson());
    }
    f.commit();
  }

  m_recentFiles->addRecentFile(savename);
  saveRecentFilesState();

  return true;
}

bool DocumentManager::saveDocumentAs(Document& doc)
{
  QFileDialog d{&m_view, tr("Save Document As")};
  QString binFilter{tr("Binary (*.scorebin)")};
  QString jsonFilter{tr("JSON (*.scorejson)")};
  QStringList filters;
  filters << jsonFilter << binFilter;

  d.setNameFilters(filters);
  d.setConfirmOverwrite(true);
  d.setFileMode(QFileDialog::AnyFile);
  d.setAcceptMode(QFileDialog::AcceptSave);

  if (d.exec())
  {
    auto files = d.selectedFiles();
    QString savename = files.first();
    auto suf = d.selectedNameFilter();

    if (!savename.isEmpty())
    {
      if (suf == binFilter)
      {
        if (!savename.contains(".scorebin"))
          savename += ".scorebin";
      }
      else
      {
        if (!savename.contains(".scorejson"))
          savename += ".scorejson";
      }

      QSaveFile f{savename};
      f.open(QIODevice::WriteOnly);
      doc.metadata().setFileName(savename);
      if (savename.indexOf(".scorebin") != -1)
        f.write(doc.saveAsByteArray());
      else
      {
        QJsonDocument json_doc;
        json_doc.setObject(doc.saveAsJson());

        f.write(json_doc.toJson());
      }
      f.commit();
    }
    return true;
  }
  return false;
}

bool DocumentManager::saveStack()
{
  QFileDialog d{&m_view, tr("Save Stack As")};
  d.setNameFilters({"*.stack"});
  d.setConfirmOverwrite(true);
  d.setFileMode(QFileDialog::AnyFile);
  d.setAcceptMode(QFileDialog::AcceptSave);

  if (d.exec())
  {
    auto files = d.selectedFiles();
    QString savename = files.first();
    if (!savename.isEmpty())
    {
      if (!savename.contains(".stack"))
        savename += ".stack";

      QSaveFile f{savename};
      f.open(QIODevice::WriteOnly);

      f.reset();
      DataStream::Serializer ser(&f);
      ser.readFrom(currentDocument()->id());
      ser.readFrom(currentDocument()->commandStack());
      f.commit();
    }
    return true;
  }
  return false;
}

Document* DocumentManager::loadStack(const iscore::GUIApplicationContext& ctx)
{
  QString loadname = QFileDialog::getOpenFileName(
      &m_view, tr("Open Stack"), QString(), "*.stack");
  if (!loadname.isEmpty() && (loadname.indexOf(".stack") != -1))
  {
    return loadStack(ctx, loadname);
  }

  return nullptr;
}

Document* DocumentManager::loadStack(
    const iscore::GUIApplicationContext& ctx, const QString& loadname)
{
  QFile cmdF{loadname};

  if (cmdF.open(QIODevice::ReadOnly))
  {
    QByteArray cmdArr{cmdF.readAll()};
    cmdF.close();

    DataStream::Deserializer writer(cmdArr);

    Id<DocumentModel> id;
    writer.writeTo(id);

    prepareNewDocument(ctx);
    auto doc = m_builder.newDocument(
        ctx, id, *ctx.interfaces<DocumentDelegateList>().begin());
    setupDocument(ctx, doc);

    loadCommandStack(
        ctx.components, writer, doc->commandStack(), [](auto cmd) {
          cmd->redo();
        });
    return doc;
  }

  return nullptr;
}

ISCORE_LIB_BASE_EXPORT
Document* DocumentManager::loadFile(const iscore::GUIApplicationContext& ctx)
{
  QString loadname = QFileDialog::getOpenFileName(
      &m_view, tr("Open"), QString(), "*.scorebin *.scorejson");
  return loadFile(ctx, loadname);
}

Document* DocumentManager::loadFile(
    const iscore::GUIApplicationContext& ctx, const QString& fileName)
{
  Document* doc{};
  if (!fileName.isEmpty() && (fileName.indexOf(".scorebin") != -1
                              || fileName.indexOf(".scorejson") != -1))
  {
    QFile f{fileName};
    if (f.open(QIODevice::ReadOnly))
    {
      m_recentFiles->addRecentFile(fileName);
      saveRecentFilesState();

      if (fileName.indexOf(".scorebin") != -1)
      {
        doc = loadDocument(
            ctx, f.readAll(),
            *ctx.interfaces<DocumentDelegateList>().begin());
      }
      else if (fileName.indexOf(".scorejson") != -1)
      {
        auto json = QJsonDocument::fromJson(f.readAll());
        bool ok = checkAndUpdateJson(json, ctx);
        if (ok)
        {
          doc = loadDocument(
              ctx, json.object(),
              *ctx.interfaces<DocumentDelegateList>().begin());
        }
        else
        {
          QMessageBox::warning(
              qApp->activeWindow(),
              tr("Unable to load"),
              tr("Unable to load file : "
                 "it has to be converted but the conversion was not "
                 "implemented yet."
                 "\n"
                 "Blame the developer."));
        }
      }

      if (doc)
      {
        m_currentDocument->metadata().setFileName(fileName);
      }
    }
  }

  return doc;
}

ISCORE_LIB_BASE_EXPORT
void DocumentManager::prepareNewDocument(const iscore::GUIApplicationContext& ctx)
{
  m_preparingNewDocument = true;
  for (GUIApplicationPlugin* appPlugin :
       ctx.guiApplicationPlugins())
  {
    appPlugin->prepareNewDocument();
  }
  m_preparingNewDocument = false;
}

bool DocumentManager::closeAllDocuments(const iscore::GUIApplicationContext& ctx)
{
  while (!m_documents.empty())
  {
    bool b = closeDocument(ctx, *m_documents.back());
    if (!b)
      return false;
  }

  return true;
}

bool DocumentManager::preparingNewDocument() const
{
  return m_preparingNewDocument;
}

bool DocumentManager::checkAndUpdateJson(
    QJsonDocument& json, const iscore::GUIApplicationContext& ctx)
{
  if (!json.isObject())
    return false;

  // Check the version
  auto obj = json.object();
  Version loaded_version{0};
  auto it = obj.find("Version");
  if (it != obj.end())
    loaded_version = Version{(*it).toInt()};

  LocalPluginVersionsMap local_plugins;
  for (const auto& plug : ctx.addons())
  {
    local_plugins.insert(plug.plugin);
  }

  std::vector<LoadedPluginVersions> loading_plugins;
  auto plugin_it = obj.find("Plugins");
  if (plugin_it != obj.end())
  {
    for (const auto& plugin_val : (*plugin_it).toObject())
    {
      const auto& plugin_obj = plugin_val.toObject();
      auto plugin_key_it = plugin_obj.find("Key");
      if (plugin_key_it == plugin_obj.end())
        continue;
      auto plugin_key
          = UuidKey<iscore::Plugin>::fromString((*plugin_key_it).toString());

      Version plugin_version{0};
      auto plugin_ver_it = plugin_obj.find("Version");
      if (plugin_ver_it != plugin_obj.end())
        plugin_version = Version{(*plugin_ver_it).toInt()};

      loading_plugins.push_back({plugin_key, plugin_version});
    }
  }

  // A file is loadable, if the main version
  // and all the plugin versions are <= to the current version,
  // and all the plug-ins are available.

  // Check the main document
  bool mainLoadable = true;
  if (loaded_version > ctx.applicationSettings.saveFormatVersion)
  {
    mainLoadable = false;
  }
  else if (loaded_version < ctx.applicationSettings.saveFormatVersion)
  {
    // TODO update main
    auto res = updateJson(
        obj, loaded_version, ctx.applicationSettings.saveFormatVersion);
    if (!res)
    {
      return false;
    }
  }

  // Check the plug-ins
  bool pluginsAvailable = true;
  bool pluginsLoadable = true;

  auto& local_map = local_plugins.get<0>();
  for (const auto& plug : loading_plugins)
  {
    auto it = local_map.find(plug.plugin);
    if (it == local_map.end())
    {
      pluginsAvailable = false;
    }
    else
    {
      auto& current_local_plugin = *it;
      if (plug.version > current_local_plugin->version())
      {
        pluginsLoadable = false;
      }
      else if (plug.version < current_local_plugin->version())
      {
        current_local_plugin->updateSaveFile(
            obj, plug.version, current_local_plugin->version());
      }
    }
  }

  json.setObject(obj);
  return mainLoadable && pluginsAvailable && pluginsLoadable;
}

bool DocumentManager::updateJson(
    QJsonObject& object, Version json_ver, Version iscore_ver)
{
  iscore::hash_map<
      Version,
      std::pair<Version, std::function<void(QJsonObject&)>>
  > conversions;
/*
  conversions.insert(
    {Version{2}, {Version{3}, [] (const QJsonObject& obj)
     {
       // Add '@' between address and accessor

     }}});

  // For now just do from n to n+1
  // TODO do the algorithm that does n..n+1..n+2..etc.

  auto it = conversions.find(json_ver);
  if(it != conversions.end())
  {
    it.value().second(object);
    return true;
  }
  */
  return false;
}

void DocumentManager::saveRecentFilesState()
{
  QSettings settings("OSSIA", "i-score");
  settings.setValue("RecentFiles", m_recentFiles->saveState());
  m_recentFiles->saveState();
}

ISCORE_LIB_BASE_EXPORT
void DocumentManager::restoreDocuments(const iscore::GUIApplicationContext& ctx)
{
  for (const auto& backup : DocumentBackups::restorableDocuments())
  {
    restoreDocument(
        ctx, backup.first, backup.second,
        *ctx.interfaces<DocumentDelegateList>().begin());
  }
}

Id<iscore::DocumentModel> getStrongId(const std::vector<iscore::Document*>& v)
{
  using namespace std;
  vector<int32_t> ids(v.size()); // Map reduce

  transform(v.begin(), v.end(), ids.begin(), [](const auto elt) {
    return elt->id().val();
  });

  return Id<iscore::DocumentModel>{
      iscore::random_id_generator::getNextId(ids)};
}
}
