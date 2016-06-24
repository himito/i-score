
#include <iscore/tools/std/Optional.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <core/document/DocumentView.hpp>
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateFactoryInterface.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateModelInterface.hpp>
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDataStream>
#include <QtGlobal>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaType>
#include <QPair>
#include <QString>
#include <QVariant>
#include <QVector>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

#include "Document.hpp"
#include "DocumentModel.hpp"
#include <iscore/application/ApplicationComponents.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <core/command/CommandStack.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/NamedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <core/presenter/DocumentManager.hpp>

class QObject;
class QWidget;


namespace iscore
{
QByteArray Document::saveDocumentModelAsByteArray()
{
    QByteArray arr;
    Serializer<DataStream> s{&arr};
    s.readFrom(model().id());
    m_model->modelDelegate().serialize(s.toVariant());
    return arr;
}

QJsonObject Document::saveDocumentModelAsJson()
{
    Serializer<JSONObject> s;
    s.m_obj["DocumentId"] = toJsonValue(model().id());
    m_model->modelDelegate().serialize(s.toVariant());
    return s.m_obj;
}

QJsonObject Document::saveAsJson()
{
    using namespace std;
    QJsonObject complete, json_plugins;

    for(const auto& plugin : model().pluginModels())
    {
        if(auto serializable_plugin = dynamic_cast<SerializableDocumentPlugin*>(plugin))
        {
            Serializer<JSONObject> s;
            s.readFrom(*serializable_plugin);
            json_plugins[serializable_plugin->objectName()] = s.m_obj;
        }
    }

    complete["Plugins"] = json_plugins;
    complete["Document"] = saveDocumentModelAsJson();

    // Indicate in the stack that the current position is saved
    m_commandStack.markCurrentIndexAsSaved();
    return complete;
}

QByteArray Document::saveAsByteArray()
{
    using namespace std;
    QByteArray global;
    QDataStream writer(&global, QIODevice::WriteOnly);

    // Save the document
    auto docByteArray = saveDocumentModelAsByteArray();

    // Save the document plug-ins
    QVector<QPair<QString, QByteArray>> documentPluginModels;

    for(const auto& plugin : model().pluginModels())
    {
        if(auto serializable_plugin = dynamic_cast<SerializableDocumentPlugin*>(plugin))
        {
            QByteArray arr;
            Serializer<DataStream> s{&arr};
            s.readFrom(*serializable_plugin);
            documentPluginModels.push_back({plugin->objectName(), arr});
        }
    }

    writer << docByteArray << documentPluginModels;

    auto hash = QCryptographicHash::hash(global, QCryptographicHash::Algorithm::Sha512);
    writer << hash;

    // Indicate in the stack that the current position is saved
    m_commandStack.markCurrentIndexAsSaved();
    return global;
}


// Load document
Document::Document(const QVariant& data,
                   DocumentDelegateFactory& factory,
                   QWidget* parentview,
                   QObject* parent):
    NamedObject {"Document", parent},
    m_objectLocker{this},
    m_context{*this}
{
    std::allocator<DocumentModel> allocator;
    m_model = allocator.allocate(1);
    try
    {
        allocator.construct(m_model, m_context, data, factory, this);
    }
    catch(...)
    {
        allocator.deallocate(m_model, 1);
        throw;
    }

    m_view = new DocumentView{factory, *this, parentview};
    m_presenter = new DocumentPresenter{factory,
                    *m_model,
                    *m_view,
                    this};
    init();
}

void DocumentModel::loadDocumentAsByteArray(
        iscore::DocumentContext& ctx,
        const QByteArray& data,
        DocumentDelegateFactory& fact)
{
    // Deserialize the first parts
    QByteArray doc;
    QVector<QPair<QString, QByteArray>> documentPluginModels;
    QByteArray hash;

    QDataStream wr{data};
    wr >> doc >> documentPluginModels >> hash;

    // Perform hash verification
    QByteArray verif_arr;
    QDataStream writer(&verif_arr, QIODevice::WriteOnly);
    writer << doc << documentPluginModels;
    if(QCryptographicHash::hash(verif_arr, QCryptographicHash::Algorithm::Sha512) != hash)
    {
        throw std::runtime_error("Invalid file.");
    }

    // Set the id

    DataStream::Deserializer doc_writer{doc};
    {
        Id<DocumentModel> doc_id;
        doc_writer.writeTo(doc_id);

        if(any_of(ctx.app.documents.documents(), [=] (auto doc) { return doc->id() == doc_id; }))
            throw std::runtime_error(tr("The document is already loaded").toStdString());

        this->setId(std::move(doc_id));
    }

    // Note : this *has* to be in this order, because
    // the plugin models might put some data in the
    // document that requires the plugin models to be loaded
    // in order to be deserialized. (e.g. the groups for the network)
    // First load the plugin models
    auto& plugin_factories = ctx.app.components.factory<DocumentPluginFactoryList>();
    Foreach(documentPluginModels, [&] (const auto& plugin_raw)
    {
        DataStream::Deserializer plug_writer{plugin_raw.second};
        auto plug = deserialize_interface(
                    plugin_factories,
                    plug_writer,
                    ctx,
                    this);

        if(plug)
        {
            this->addPluginModel(plug);
        }
    });

    // Load the document model
    m_model = fact.loadModel(doc_writer.toVariant(), ctx, this);
}

void DocumentModel::loadDocumentAsJson(
        iscore::DocumentContext& ctx,
        const QJsonObject& json,
        DocumentDelegateFactory& fact)
{
    const auto& doc = json["Document"].toObject();
    auto doc_id = fromJsonValue<Id<DocumentModel>>(doc["DocumentId"]);

    if(any_of(ctx.app.documents.documents(), [=] (auto doc) { return doc->id() == doc_id; }))
        throw std::runtime_error(tr("The document is already loaded").toStdString());

    this->setId(doc_id);

    // Load the plug-in models
    auto json_plugins = json["Plugins"].toObject();
    auto& plugin_factories = ctx.app.components.factory<DocumentPluginFactoryList>();
    Foreach(json_plugins.keys(), [&] (const auto& key)
    {
        JSONObject::Deserializer plug_writer{json_plugins[key].toObject()};
        auto plug = deserialize_interface(
                    plugin_factories,
                    plug_writer,
                    ctx,
                    this);

        if(plug)
        {
            this->addPluginModel(plug);
        }
    });

    // Load the model
    JSONObject::Deserializer doc_writer{doc};
    m_model = fact.loadModel(doc_writer.toVariant(), ctx, this);
}

// Load document model
DocumentModel::DocumentModel(
        iscore::DocumentContext& ctx,
        const QVariant& data,
        DocumentDelegateFactory& fact,
        QObject* parent) :
    IdentifiedObject {Id<DocumentModel>(iscore::id_generator::getFirstId()), "DocumentModel", parent}
{
    using namespace std;
    if(data.canConvert(QMetaType::QByteArray))
    {
        loadDocumentAsByteArray(ctx, data.toByteArray(), fact);
    }
    else if(data.canConvert(QMetaType::QJsonObject))
    {
        loadDocumentAsJson(ctx, data.toJsonObject(), fact);
    }
    else
    {
        ISCORE_ABORT;
    }
}
}
