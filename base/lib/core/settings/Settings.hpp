#pragma once
#include <QObject>
#include <QSettings>
#include <iscore_lib_base_export.h>
#include <memory>
namespace iscore
{
class SettingsDelegateFactory;
class SettingsDelegateModel;
class SettingsModel;
class SettingsPresenter;
class SettingsView;

struct ApplicationContext;
} // namespace iscore

namespace iscore
{
/**
 * @brief Application-wide user settings registering and handling.
 *
 * Settings do not fit in the other MVP parts of the software due to the
 * command application difference.
 *
 * When "ok" is pressed in the settings panel, the plug-ins are required
 * to commit their changes to their respective models.
 * Else they discard them.
 *
 * A plug-in wishing to add user settings must :
 *
 * * Implement iscore::SettingsDelegateModel, iscore::SettingsDelegatePresenter, iscore::SettingsDelegateView
 * * Register them with ISCORE_DECLARE_SETTINGS_FACTORY.
 *
 */
class ISCORE_LIB_BASE_EXPORT Settings final
{
public:
  Settings();
  ~Settings();

  Settings(const Settings&) = delete;
  Settings(Settings&&) = delete;
  Settings& operator=(const Settings&) = delete;
  Settings& operator=(Settings&&) = delete;

  void setupSettingsPlugin(
      QSettings& s,
      const iscore::ApplicationContext& ctx,
      SettingsDelegateFactory& plugin);
  SettingsView& view() const
  {
    return *m_settingsView;
  }

  auto& settings() const
  {
    return m_settings;
  }

private:
  SettingsView* m_settingsView{};
  SettingsPresenter* m_settingsPresenter{};

  std::vector<std::unique_ptr<SettingsDelegateModel>> m_settings;
};
}
