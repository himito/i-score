#include <QAction>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <core/view/View.hpp>
#include <iscore/plugins/application/GUIApplicationPlugin.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <qnamespace.h>

#include <QString>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <QApplication>

#include "QRecentFilesMenu.h"
#include <core/document/Document.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <iscore/application/ApplicationComponents.hpp>

#include <core/presenter/Presenter.hpp>
#include <core/settings/Settings.hpp>
#include <core/settings/SettingsView.hpp>
#include <iscore/actions/Menu.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateFactory.hpp>

#include <iscore/model/Identifier.hpp>

#include "iscore_git_info.hpp"
#include <iscore/plugins/panel/PanelDelegate.hpp>

namespace iscore
{
class Document;
class DocumentModel;
} // namespace iscore

namespace iscore
{

Presenter::Presenter(
    const iscore::ApplicationSettings& app,
    const iscore::Settings& set,
    View* view,
    QObject* arg_parent)
    : QObject{arg_parent}
    , m_view{view}
    , m_settings{set}
    , m_docManager{*view, this}
    , m_components{}
    , m_components_readonly{m_components}
    ,
#ifdef __APPLE__
    m_menubar{new QMenuBar}
    ,
#else
    m_menubar{view->menuBar()}
    ,
#endif
    m_context{
        app,       m_components_readonly, m_docManager, m_menus, m_toolbars,
        m_actions, m_settings.settings(), *m_view}
{
  m_docManager.init(m_context); // It is necessary to break
  // this dependency cycle.

  connect(
      &m_context.docManager, &DocumentManager::documentChanged, &m_actions,
      &ActionManager::reset);

  m_view->setPresenter(this);
}

bool Presenter::exit()
{
  return m_docManager.closeAllDocuments(m_context);
}

View* Presenter::view() const
{
  return m_view;
}

void Presenter::setupGUI()
{
  // 1. Show the menus
  // If the menu has no parent menu, we add it to the main menu bar.
  {
    std::vector<Menu> menus;
    menus.reserve(m_menus.get().size());
    for (auto& elt : m_menus.get())
    {
      if (elt.second.toplevel())
        menus.push_back(elt.second);
    }
    std::sort(menus.begin(), menus.end(), [](auto& lhs, auto& rhs) {
      return lhs.column() < rhs.column();
    });

    for (Menu& menu : menus)
    {
      view()->menuBar()->addMenu(menu.menu());
    }
  }

  // 2. Show the toolbars
  // Put them in a matrix corresponding to their organization
  {
    std::vector<std::vector<Toolbar>> toolbars;
    auto it = std::max_element(
        m_toolbars.get().begin(), m_toolbars.get().end(),
        [](auto& lhs, auto& rhs) {
          return lhs.second.row() < rhs.second.row();
        });
    if (it != m_toolbars.get().end())
    {
      toolbars.resize(it->second.row() + 1);

      for (auto& tb : m_toolbars.get())
      {
        toolbars.at(tb.second.row()).push_back(tb.second);
      }

      int i = 0;
      int n = toolbars.size();
      for (auto& tb_row : toolbars)
      {
        std::sort(tb_row.begin(), tb_row.end(), [](auto& lhs, auto& rhs) {
          return lhs.row() < rhs.row();
        });
        for (Toolbar& tb : tb_row)
          view()->addToolBar(Qt::TopToolBarArea, tb.toolbar());

        i++;
        if (i < n - 1)
          view()->addToolBarBreak(Qt::TopToolBarArea);
      }
    }
  }
}


void Presenter::optimize()
{
  iscore::optimize_hash_map(m_components.commands);
  auto& com = m_components.commands;
  auto com_end = com.end();
  for(auto it = com.begin(); it != com_end; ++it)
  {
    iscore::optimize_hash_map(it.value());
  }

  iscore::optimize_hash_map(m_components.factories);
  for(auto& fact : m_components.factories)
  {
    fact.second->optimize();
  }

  iscore::optimize_hash_map(m_menus.get());
  iscore::optimize_hash_map(m_actions.get());
  iscore::optimize_hash_map(m_toolbars.get());
}
}
