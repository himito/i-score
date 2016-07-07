#pragma once

#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>


namespace Scenario
{
class ConstraintModel;
class EventModel;
class ProcessModel;
class TimeNodeModel;
ISCORE_PLUGIN_SCENARIO_EXPORT void updateTimeNodeExtent(
        const Id<TimeNodeModel>& id,
        Scenario::ProcessModel& s);

// Will call updateTimeNodeExtent
ISCORE_PLUGIN_SCENARIO_EXPORT void updateEventExtent(
        const Id<EventModel>& id,
        Scenario::ProcessModel& s);

// Will call updateEventExtent
ISCORE_PLUGIN_SCENARIO_EXPORT void updateConstraintVerticalPos(
        double y,
        const Id<ConstraintModel>& id,
        Scenario::ProcessModel& s);
}
