#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegatePresenter.hpp>

namespace Engine
{
namespace LocalTree
{
namespace Settings
{
class Model;
class View;
class Presenter : public iscore::SettingsDelegatePresenter
{
public:
  using model_type = Model;
  using view_type = View;
  Presenter(Model&, View&, QObject* parent);

private:
  QString settingsName() override;
  QIcon settingsIcon() override;
};
}
}
}
