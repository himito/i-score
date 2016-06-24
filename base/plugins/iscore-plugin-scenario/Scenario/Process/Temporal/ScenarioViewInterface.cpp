#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintView.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <QtGlobal>
#include <QPoint>
#include <QRect>

#include <Process/LayerModel.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintPresenter.hpp>
#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintPresenter.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Event/EventPresenter.hpp>
#include <Scenario/Document/Event/EventView.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/State/StatePresenter.hpp>
#include <Scenario/Document/State/StateView.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodePresenter.hpp>
#include <Scenario/Document/TimeNode/TimeNodeView.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockView.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockPresenter.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioView.hpp>
#include "ScenarioViewInterface.hpp"
#include "TemporalScenarioPresenter.hpp"
#include <iscore/tools/IdentifiedObjectMap.hpp>

#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
ScenarioViewInterface::ScenarioViewInterface(const TemporalScenarioPresenter& presenter) :
    m_presenter{presenter}
{
}

void ScenarioViewInterface::on_eventMoved(const EventPresenter& ev)
{
    auto h = m_presenter.m_view->boundingRect().height();

    ev.view()->setExtent(ev.model().extent() * h);

    ev.view()->setPos({ev.model().date().toPixels(m_presenter.m_zoomRatio),
                             ev.model().extent().top() * h});

    // We also have to move all the relevant states
    for(const auto& state : ev.model().states())
    {
        auto state_it = m_presenter.m_states.find(state);
        if(state_it != m_presenter.m_states.end())
        {
            on_stateMoved(*state_it);
        }
    }
    m_presenter.m_view->update();
}

void ScenarioViewInterface::on_constraintMoved(const TemporalConstraintPresenter& pres)
{
    auto rect = m_presenter.m_view->boundingRect();
    auto msPerPixel = m_presenter.m_zoomRatio;

    const auto& cstr_model = pres.model();
    auto& cstr_view = view(pres);

    double startPos = cstr_model.startDate().toPixels(msPerPixel);
    // double delta = cstr_view.x() - startPos;
    bool dateChanged = true; // Disabled because it does a whacky movement when there are processes. (delta * delta > 1); // Magnetism

    if(dateChanged)
    {
        cstr_view.setPos(startPos,
                         rect.height() * cstr_model.heightPercentage());
    }
    else
    {
        cstr_view.setY(qreal(rect.height() * cstr_model.heightPercentage()));
    }

    cstr_view.setDefaultWidth(cstr_model.duration.defaultDuration().toPixels(msPerPixel));
    cstr_view.setMinWidth(cstr_model.duration.minDuration().toPixels(msPerPixel));
    cstr_view.setMaxWidth(cstr_model.duration.maxDuration().isInfinite(),
                          cstr_model.duration.maxDuration().isInfinite()
                            ? -1
                            : cstr_model.duration.maxDuration().toPixels(msPerPixel));

    m_presenter.m_view->update();
}

void ScenarioViewInterface::on_timeNodeMoved(const TimeNodePresenter &timenode)
{
    auto h = m_presenter.m_view->boundingRect().height();
    timenode.view()->setExtent(timenode.model().extent() * h);

    timenode.view()->setPos({timenode.model().date().toPixels(m_presenter.m_zoomRatio),
                             timenode.model().extent().top() * h});

    m_presenter.m_view->update();
}

void ScenarioViewInterface::on_stateMoved(const StatePresenter& state)
{
    auto rect = m_presenter.m_view->boundingRect();
    const auto& ev = static_cast<const Scenario::ScenarioModel&>(m_presenter.layerModel().processModel()).event(state.model().eventId());

    state.view()->setPos({ev.date().toPixels(m_presenter.m_zoomRatio),
                          rect.height() * state.model().heightPercentage()});

    m_presenter.m_view->update();
}

void ScenarioViewInterface::on_commentMoved(const CommentBlockPresenter& comment)
{
    auto h = m_presenter.m_view->boundingRect().height();
    comment.view()->setPos(comment.date().toPixels(m_presenter.zoomRatio()),
                           comment.model().heightPercentage() * h);
    m_presenter.m_view->update();
}

template<typename T>
void update_min_max(const T& val, T& min, T& max)
{
    min = val < min ? val : min;
    max = val > max ? val : max;
}

void ScenarioViewInterface::on_hoverOnConstraint(const Id<ConstraintModel>& constraintId, bool enter)
{
    /*
    const auto& constraint = m_presenter.m_constraints.at(constraintId)->model();
    EventPresenter* start = m_presenter.m_events.at(constraint.startEvent());
    start->view()->setShadow(enter);
    EventPresenter* end = m_presenter.m_events.at(constraint.endEvent());
    end->view()->setShadow(enter);
    */
}

void ScenarioViewInterface::on_hoverOnEvent(const Id<EventModel>& eventId, bool enter)
{
    /*
    const auto& event = m_presenter.m_events.at(eventId)->model();
    for (const auto& cstr : event.constraints())
    {
        auto cstrView = view(m_presenter.m_constraints.at(cstr));
        cstrView->setShadow(enter);
    }
    */
}

void ScenarioViewInterface::on_graphicalScaleChanged(double scale)
{
    for(auto& e : m_presenter.getEvents())
    {
        e.view()->setWidthScale(scale);
    }
    for(auto& s : m_presenter.getStates())
    {
        s.view()->setScale(scale);
    }
    for(auto& c : m_presenter.getConstraints())
    {
        auto& cstr_view = view(c);
        cstr_view.setHeightScale(scale);
    }

    m_presenter.m_view->update();
}
}
