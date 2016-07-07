#include <Scenario/Process/Temporal/TemporalScenarioLayerModel.hpp>

#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <QDataStream>
#include <QDebug>
#include <QtGlobal>
#include <QIODevice>
#include <QMap>
#include <vector>

#include "Algorithms/StandardCreationPolicy.hpp"
#include <Process/ModelMetadata.hpp>
#include <Process/Process.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <Scenario/Process/ScenarioProcessMetadata.hpp>
#include "ScenarioModel.hpp"
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/Todo.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>

namespace Process { class LayerModel; }
class ProcessStateDataInterface;

namespace Scenario
{
// RENAMEME and my header too
ProcessModel::ProcessModel(const TimeValue& duration,
                           const Id<Process::ProcessModel>& id,
                           QObject* parent) :
    Process::ProcessModel {duration, id, Metadata<ObjectKey_k, Scenario::ProcessModel>::get(), parent},
    m_startTimeNodeId{Scenario::startId<TimeNodeModel>()},
    m_endTimeNodeId{Scenario::endId<TimeNodeModel>()},
    m_startEventId{Scenario::startId<EventModel>()},
    m_endEventId{Scenario::endId<EventModel>()},
    m_startStateId{Scenario::startId<StateModel>()}
{
    auto& start_tn = ScenarioCreate<TimeNodeModel>::redo(m_startTimeNodeId, {0., 0.1}, TimeValue::zero(), *this);
    auto& end_tn = ScenarioCreate<TimeNodeModel>::redo(m_endTimeNodeId, {0., 0.1}, duration, *this);

    auto& start_ev = ScenarioCreate<EventModel>::redo(m_startEventId, start_tn, {0., 0.0}, *this);
    ScenarioCreate<EventModel>::redo(m_endEventId, end_tn, {0., 0.}, *this);

    ScenarioCreate<StateModel>::redo(m_startStateId, start_ev, 0.02, *this);

    // At the end because plug-ins depend on the start/end timenode & al being here
    pluginModelList = new iscore::ElementPluginModelList{iscore::IDocument::documentContext(*parent), this};
    metadata.setName(QString("Scenario.%1").arg(*this->id().val()));
}

ProcessModel::ProcessModel(
        const Scenario::ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent) :
    Process::ProcessModel {source, id, Metadata<ObjectKey_k, Scenario::ProcessModel>::get(), parent},
    m_startTimeNodeId{source.m_startTimeNodeId},
    m_endTimeNodeId{source.m_endTimeNodeId},
    m_startEventId{source.m_startEventId},
    m_endEventId{source.m_endEventId}
{
    pluginModelList = new iscore::ElementPluginModelList(*source.pluginModelList, this);

    // This almost terrifying piece of code will simply clone
    // all the elements (constraint, etc...) from the source to this class
    // without duplicating code too much.
    auto clone = [&] (const auto& m) {
        using the_class = typename remove_qualifs_t<decltype(this->*m)>::value_type;
        for(const auto& elt : source.*m)
            (this->*m).add(new the_class{elt, elt.id(), this});
    };
    clone(&ProcessModel::timeNodes);
    clone(&ProcessModel::events);
    clone(&ProcessModel::constraints);
    clone(&ProcessModel::comments);
    auto& stack = iscore::IDocument::documentContext(*this).commandStack;
    for(const auto& elt : source.states)
    {
        auto st = new StateModel{elt, elt.id(), stack, this};
        states.add(st);
    }

    // We re-set the constraints before and after the states
    for(const Scenario::ConstraintModel& constraint : constraints)
    {
        Scenario::SetPreviousConstraint(states.at(constraint.endState()), constraint);
        Scenario::SetNextConstraint(states.at(constraint.startState()), constraint);
    }

    metadata.setName(QString("Scenario.%1").arg(*this->id().val()));
}

ProcessModel::~ProcessModel()
{
    for(auto elt : this->components.map().get())
        delete elt;

    apply([&] (const auto& m) {
        for(auto elt : (this->*m).map().get())
            delete elt;
    });
    delete pluginModelList;
}

void ProcessModel::setDurationAndScale(const TimeValue& newDuration)
{
    double scale =  newDuration / duration();

    for(auto& timenode : timeNodes)
    {
        timenode.setDate(timenode.date() * scale);
        // Since events will also move we do not need
        // to move the timenode.
    }

    for(auto& event : events)
    {
        event.setDate(event.date() * scale);
        emit eventMoved(event);
    }
    for(auto& cmt : comments)
    {
        cmt.setDate(cmt.date() * scale);
    }

    for(auto& constraint : constraints)
    {
        constraint.setStartDate(constraint.startDate() * scale);
        // Note : scale the min / max.

        auto newdur = constraint.duration.defaultDuration() * scale;
        ConstraintDurations::Algorithms::scaleAllDurations(
                    constraint, newdur);

        for(auto& process : constraint.processes)
        {
            process.setParentDuration(ExpandMode::Scale, newdur);
        }

        emit constraintMoved(constraint);
    }

    this->setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeValue& newDuration)
{
    // TODO what happens when there are constraints linked here ?
    auto& eev = endEvent();

    eev.setDate(newDuration);
    timeNode(eev.timeNode()).setDate(newDuration);
    emit eventMoved(eev);
    this->setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeValue& newDuration)
{
    this->setDuration(newDuration);
    return; // Disabled by Asana
}

void ProcessModel::startExecution()
{
    // TODO this is called for each process!!
    // But it should be done only once at the global level.
    emit execution(true);
    for(ConstraintModel& constraint : constraints)
    {
        constraint.startExecution();
    }
}

void ProcessModel::stopExecution()
{
    emit execution(false);
    for(ConstraintModel& constraint : constraints)
    {
        constraint.stopExecution();
    }
}

void ProcessModel::reset()
{
    for(auto& constraint : constraints)
    {
        constraint.reset();
    }

    for(auto& event : events)
    {
        event.reset();
    }

    // TODO reset events / states display too
}

Selection ProcessModel::selectableChildren() const
{
    Selection objects;
    apply([&] (const auto& m) {
        for(auto& elt : this->*m)
            objects.append(&elt);
    });
    return objects;
}

template<typename InputVec, typename OutputVec>
static void copySelected(const InputVec& in, OutputVec& out)
{
    for(const auto& elt : in)
    {
        if(elt.selection.get())
            out.append(&elt);
    }
}

Selection ProcessModel::selectedChildren() const
{
    Selection objects;
    apply([&] (const auto& m) { copySelected(this->*m, objects); });
    return objects;
}

void ProcessModel::setSelection(const Selection& s) const
{
    // OPTIMIZEME
    apply([&] (auto&& m) {
        for(auto& elt : this->*m)
            elt.selection.set(s.contains(&elt));
    });
}

void ProcessModel::setupLayer(AbstractScenarioLayerModel* scen)
{
    // There is no ConstraintCreated connection to the layer,
    // because the constraints view models are created
    // from the commands, since they require ids too.
    constraints.removed.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::on_constraintRemoved>(scen);

    states.added.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::stateCreated>(scen);
    states.removed.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::stateRemoved>(scen);

    events.added.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::eventCreated>(scen);
    events.removed.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::eventRemoved>(scen);

    timeNodes.added.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::timeNodeCreated>(scen);
    timeNodes.removed.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::timeNodeRemoved>(scen);

    comments.added.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::commentCreated>(scen);
    comments.removed.connect<AbstractScenarioLayerModel, &AbstractScenarioLayerModel::commentRemoved>(scen);

    connect(this, &ProcessModel::eventMoved,
            scen, &AbstractScenarioLayerModel::eventMoved);

    connect(this, &ProcessModel::constraintMoved,
            scen, &AbstractScenarioLayerModel::constraintMoved);

    connect(this, &ProcessModel::commentMoved,
            scen, &AbstractScenarioLayerModel::commentMoved);
}


const QVector<Id<ConstraintModel> > constraintsBeforeTimeNode(
        const Scenario::ProcessModel& scenar,
        const Id<TimeNodeModel>& timeNodeId)
{
    QVector<Id<ConstraintModel>> cstrs;
    const auto& tn = scenar.timeNodes.at(timeNodeId);
    for(const auto& ev : tn.events())
    {
        const auto& evM = scenar.events.at(ev);
        for (const auto& st : evM.states())
        {
            const auto& stM = scenar.states.at(st);
            if(stM.previousConstraint())
                cstrs.push_back(stM.previousConstraint());
        }
    }

    return cstrs;
}

const StateModel* furthestSelectedState(const Scenario::ProcessModel& scenar)
{
    const StateModel* furthest_state{};
    {
        TimeValue max_t = TimeValue::zero();
        double max_y = 0;
        for(StateModel& state : scenar.states)
        {
            if(state.selection.get())
            {
                auto date = scenar.events.at(state.eventId()).date();
                if(!furthest_state || date > max_t)
                {
                    max_t = date;
                    max_y = state.heightPercentage();
                    furthest_state = &state;
                }
                else if(date == max_t && state.heightPercentage() > max_y)
                {
                    max_y = state.heightPercentage();
                    furthest_state = &state;
                }
            }
        }
        if(furthest_state)
        {
            return furthest_state;
        }
    }

    // If there is no furthest state, we instead go for a constraint
    const ConstraintModel* furthest_constraint{};
    {
        TimeValue max_t = TimeValue::zero();
        double max_y = 0;
        for(ConstraintModel& cst : scenar.constraints)
        {
            if(cst.selection.get())
            {
                auto date = cst.duration.defaultDuration();
                if(!furthest_constraint || date > max_t)
                {
                    max_t = date;
                    max_y = cst.heightPercentage();
                    furthest_constraint = &cst;
                }
                else if (date == max_t && cst.heightPercentage() > max_y)
                {
                    max_y = cst.heightPercentage();
                    furthest_constraint = &cst;
                }
            }
        }

        if (furthest_constraint)
        {
            return &scenar.states.at(furthest_constraint->endState());
        }
    }

    return nullptr;
}

const StateModel* furthestSelectedStateWithoutFollowingConstraint(const Scenario::ProcessModel& scenar)
{
    const StateModel* furthest_state{};
    {
        TimeValue max_t = TimeValue::zero();
        double max_y = 0;
        for(StateModel& state : scenar.states)
        {
            if(state.selection.get() && !state.nextConstraint())
            {
                auto date = scenar.events.at(state.eventId()).date();
                if(!furthest_state || date > max_t)
                {
                    max_t = date;
                    max_y = state.heightPercentage();
                    furthest_state = &state;
                }
                else if(date == max_t && state.heightPercentage() > max_y)
                {
                    max_y = state.heightPercentage();
                    furthest_state = &state;
                }
            }
        }
        if(furthest_state)
        {
            return furthest_state;
        }
    }

    // If there is no furthest state, we instead go for a constraint
    const ConstraintModel* furthest_constraint{};
    {
        TimeValue max_t = TimeValue::zero();
        double max_y = 0;
        for(ConstraintModel& cst : scenar.constraints)
        {
            if(cst.selection.get())
            {
                const auto& state = scenar.states.at(cst.endState());
                if(state.nextConstraint())
                    continue;

                auto date = cst.duration.defaultDuration();
                if(!furthest_constraint || date > max_t)
                {
                    max_t = date;
                    max_y = cst.heightPercentage();
                    furthest_constraint = &cst;
                }
                else if (date == max_t && cst.heightPercentage() > max_y)
                {
                    max_y = cst.heightPercentage();
                    furthest_constraint = &cst;
                }
            }
        }

        if (furthest_constraint)
        {
            return &scenar.states.at(furthest_constraint->endState());
        }
    }

    return nullptr;
}

}
