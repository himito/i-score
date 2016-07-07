#pragma once
#include <Process/LayerModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModelIdMap.hpp>
#include <QString>
#include <QVector>
#include <vector>
#include <nano_signal_slot.hpp>

#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Process { class ProcessModel; }
class QObject;
namespace Scenario
{
class StateModel;
class TimeNodeModel;
class ConstraintModel;
class ConstraintViewModel;
class EventModel;
class ProcessModel;

class AbstractScenarioLayerModel :
        public Process::LayerModel,
        public Nano::Observer
{
        Q_OBJECT
    public:
        using model_type = Scenario::ProcessModel;

        virtual void makeConstraintViewModel(
                const Id<ConstraintModel>& constraintModelId,
                const Id<ConstraintViewModel>& constraintViewModelId) = 0;

        void removeConstraintViewModel(
                const Id<ConstraintViewModel>& constraintViewModelId);

        // Access to elements
        // A given constraint can be represented only once in a given scenario VM, hence...
        ConstraintViewModel& constraint(
                const Id<ConstraintModel>& constraintModelid) const;
        ConstraintViewModel& constraint(
                const Id<ConstraintViewModel>& constraintViewModelid) const;
        QVector<ConstraintViewModel*> constraints() const;

    signals:
        // "created" signal is in the relevant subclasses
        void constraintViewModelRemoved(const ConstraintViewModel&);

        void stateCreated(const StateModel&);
        void stateRemoved(const StateModel&);

        void eventCreated(const EventModel&);
        void eventRemoved(const EventModel&);

        void timeNodeCreated(const TimeNodeModel&);
        void timeNodeRemoved(const TimeNodeModel&);

        void commentCreated(const CommentBlockModel&);
        void commentRemoved(const CommentBlockModel&);

        void eventMoved(const EventModel&);
        void constraintMoved(const ConstraintModel&);
        void commentMoved(const CommentBlockModel&);

    public slots:
        virtual void on_constraintRemoved(const ConstraintModel&) = 0;

    protected:
        AbstractScenarioLayerModel(const Id<LayerModel>& viewModelId,
                              const QString& name,
                              Process::ProcessModel& sharedProcess,
                              QObject* parent) :
            LayerModel {viewModelId,
                        name,
                        sharedProcess,
                        parent
}
        {
        }

        // Copy
        AbstractScenarioLayerModel(const AbstractScenarioLayerModel& source,
                              const Id<LayerModel>& viewModelId,
                              const QString& name,
                              Process::ProcessModel& sharedProcess,
                              QObject* parent) :
            LayerModel {viewModelId,
                        name,
                        sharedProcess,
                        parent}
        {
        }

        // Load
        template<typename Impl>
        AbstractScenarioLayerModel(Deserializer<Impl>& vis,
                              Process::ProcessModel& sharedProcess,
                              QObject* parent) :
            LayerModel {vis,
                        sharedProcess,
                        parent}
        {
            // No data to save (the constraints vector will be rebuilt by the subclass accordingly.)
        }

        QVector<ConstraintViewModel*> m_constraints;
};


// TODO these methods should go in another file.
template<typename T>
typename T::constraint_layer_type& constraintViewModel(
        const T& scenarioViewModel,
        const Id<ConstraintViewModel>& cvm_id)
{
    return static_cast<typename T::constraint_layer_type&>(scenarioViewModel.constraint(cvm_id));
}


template<typename T>
QVector<typename T::constraint_layer_type*> constraintsViewModels(const T& scenarioViewModel)
{
    QVector<typename T::constraint_layer_type*> v;

    for(auto& elt : scenarioViewModel.constraints())
    {
        v.push_back(static_cast<typename T::constraint_layer_type*>(elt));
    }

    return v;
}

void createConstraintViewModels(const ConstraintViewModelIdMap& idMap,
                                const Id<ConstraintModel>& constraint,
                                const Scenario::ProcessModel& scenario);

// Note : the view models can also be more easily accessed using the viewModels methods of ConstraintModel
std::vector<ConstraintViewModel*> getConstraintViewModels(
        const Id<ConstraintModel>& constraintId,
        const Scenario::ProcessModel& scenario);
}
