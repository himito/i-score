#pragma once

#include <State/Address.hpp>
#include <Process/TimeValue.hpp>
#include <Device/Address/AddressSettings.hpp>

#include <iscore/command/Dispatchers/MacroCommandDispatcher.hpp>

#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace RedoStrategy {
struct Quiet;
}
namespace Explorer
{
class DeviceExplorerModel;
}
namespace Scenario {
class ProcessModel;
class ConstraintModel;
class EventModel;
class RackModel;
class SlotModel;
struct Point;
namespace Command
{
class MoveNewEvent;
}
}

namespace std
{
template <>
struct hash<Device::FullAddressSettings>
{
        std::size_t operator()(const Device::FullAddressSettings& k) const
        {
            return std::hash<State::Address>{}(k.address);
        }
};
}

namespace Recording
{
using RecordCommandDispatcher = GenericMacroCommandDispatcher<
RedoStrategy::Quiet,
SendStrategy::UndoRedo>;

struct Box
{
        Scenario::ConstraintModel& constraint;
        Scenario::RackModel& rack;
        Scenario::SlotModel& slot;

        Scenario::Command::MoveNewEvent& moveCommand;
        Id<Scenario::EventModel> endEvent;
};

// Only the selected addresses
std::vector<std::vector<Device::FullAddressSettings>> GetAddressesToRecord(
        Explorer::DeviceExplorerModel& m_explorer);

// The selected addresses and all their children
std::vector<std::vector<Device::FullAddressSettings> > GetAddressesToRecordRecursive(
        Explorer::DeviceExplorerModel& explorer);

Box CreateBox(
        const Scenario::ProcessModel& scenar,
        Scenario::Point pt,
        RecordCommandDispatcher& dispatcher);


inline double GetTimeDifferenceInDouble(std::chrono::steady_clock::time_point start)
{
    using namespace std::chrono;
    return duration_cast<microseconds>(
               steady_clock::now() - start).count() / 1000.;
}
inline TimeValue GetTimeDifference(std::chrono::steady_clock::time_point start)
{
    using namespace std::chrono;
    return TimeValue::fromMsecs(GetTimeDifferenceInDouble(start));
}
}
