#pragma once
#include <Curve/Settings/CurveSettingsModel.hpp>
#include <iscore/plugins/settingsdelegate/SettingsDelegateView.hpp>
class QCheckBox;
class QDoubleSpinBox;
namespace Curve
{
namespace Settings
{

class View : public iscore::SettingsDelegateView
{
  Q_OBJECT
public:
  View();

  void setSimplificationRatio(double);
  void setSimplify(bool);
  void setMode(Mode);
  void setPlayWhileRecording(bool);

signals:
  void simplificationRatioChanged(double);
  void simplifyChanged(bool);
  void modeChanged(Mode);
  void playWhileRecordingChanged(bool);

private:
  QWidget* getWidget() override;
  QWidget* m_widg{};

  QDoubleSpinBox* m_sb{};
  QCheckBox* m_simpl{};
  QCheckBox* m_mode{};
  QCheckBox* m_playWhileRecording{};
};
}
}
