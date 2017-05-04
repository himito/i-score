#include <QApplication>
#include <QDebug>
#include <QSet>
#include <QSettings>
#include <QStandardItemModel>
#include <QStringList>
#include <QVariant>
#include <qnamespace.h>

#include "PluginSettingsModel.hpp"
#include "commands/BlacklistCommand.hpp"
#include <ossia/detail/algorithms.hpp>
#include <iscore/plugins/settingsdelegate/SettingsDelegateModel.hpp>
#include <iscore/application/ApplicationContext.hpp>
namespace PluginSettings
{
PluginSettingsModel::PluginSettingsModel(
    QSettings& set, const iscore::ApplicationContext& ctx)
    : iscore::SettingsDelegateModel{}
    , localPlugins{ctx.addons()}
    , remoteSelection{&remotePlugins}
{
}
}
