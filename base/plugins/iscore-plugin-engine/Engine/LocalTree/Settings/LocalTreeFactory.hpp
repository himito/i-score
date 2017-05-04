#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateFactory.hpp>

#include <Engine/LocalTree/Settings/LocalTreeModel.hpp>
#include <Engine/LocalTree/Settings/LocalTreePresenter.hpp>
#include <Engine/LocalTree/Settings/LocalTreeView.hpp>

namespace Engine
{
namespace LocalTree
{
namespace Settings
{
ISCORE_DECLARE_SETTINGS_FACTORY(
    Factory, Model, Presenter, View, "3cf335f6-8f5d-401b-98a3-eedfd5e7d292")
}
}
}
