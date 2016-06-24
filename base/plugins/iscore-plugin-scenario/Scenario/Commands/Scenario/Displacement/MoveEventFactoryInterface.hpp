#pragma once

#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>

template <typename Object> class Path;
namespace Scenario
{
class ScenarioModel;
class EventModel;
class ScenarioModel;
namespace Command
{
class SerializableMoveEvent;
class ISCORE_PLUGIN_SCENARIO_EXPORT MoveEventFactoryInterface :
        public iscore::AbstractFactory<MoveEventFactoryInterface>
{
        ISCORE_ABSTRACT_FACTORY_DECL(
                MoveEventFactoryInterface,
                "69dc1f79-5cb9-4a36-b382-8c099f7abf57")
public:
        enum Strategy{CREATION, MOVE};
    virtual SerializableMoveEvent* make(
            Path<Scenario::ScenarioModel>&& scenarioPath,
            Id<EventModel> eventId,
            TimeValue newDate,
            ExpandMode mode) = 0;

    virtual SerializableMoveEvent* make() = 0;

    virtual ~MoveEventFactoryInterface();

     /**
     * @brief priority
     * the highest priority will be default move behavior for the indicated strategy
     * Basically, we want to know how well the policy is suited for the desired strategy.
     * @param strategy
     * the strategy for which we need a displacement policy;
     * @return
     */
    virtual int priority(const iscore::ApplicationContext& ctx, Strategy strategy) const = 0;

};
}
}

