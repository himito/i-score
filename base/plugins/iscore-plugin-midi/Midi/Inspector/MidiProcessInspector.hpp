#pragma once
#include <Midi/MidiProcess.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
class QComboBox;
class QSpinBox;
namespace Midi
{
class InspectorWidget final
    : public Process::InspectorWidgetDelegate_T<Midi::ProcessModel>
{
public:
  explicit InspectorWidget(
      const ProcessModel& object,
      const iscore::DocumentContext& context,
      QWidget* parent);

private:
  void on_deviceChange(const QString& dev);

  QComboBox* m_devices{};
  QSpinBox* m_chan{};
};
class InspectorFactory final
    : public Process::
          InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
  ISCORE_CONCRETE("78f380ff-a405-47b6-9d3b-7022af996199")
};
}
