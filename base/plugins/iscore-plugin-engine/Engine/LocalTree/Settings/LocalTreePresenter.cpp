#include "LocalTreePresenter.hpp"
#include "LocalTreeModel.hpp"
#include "LocalTreeView.hpp"
#include <QApplication>
#include <QStyle>
#include <iscore/command/Dispatchers/ICommandDispatcher.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/command/SettingsCommand.hpp>

namespace Engine
{
namespace LocalTree
{
namespace Settings
{
Presenter::Presenter(Model& m, View& v, QObject* parent)
    : iscore::SettingsDelegatePresenter{m, v, parent}
{
  con(v, &View::localTreeChanged, this, [&](auto val) {
    if (val != m.getLocalTree())
    {
      m_disp.submitDeferredCommand<SetModelLocalTree>(this->model(this), val);
    }
  });

  con(m, &Model::LocalTreeChanged, &v, &View::setLocalTree);
  v.setLocalTree(m.getLocalTree());
}

QString Presenter::settingsName()
{
  return tr("Local tree");
}

QIcon Presenter::settingsIcon()
{
  return QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
}
}
}
}
