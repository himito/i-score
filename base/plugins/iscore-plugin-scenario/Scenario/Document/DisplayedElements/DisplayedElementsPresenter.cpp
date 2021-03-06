#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentView.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <iscore/tools/std/Optional.hpp>

#include "DisplayedElementsPresenter.hpp"
#include <ossia/detail/algorithms.hpp>
#include <Process/TimeValue.hpp>
#include <Process/ZoomHelper.hpp>
#include <Scenario/Document/BaseScenario/BaseScenarioPresenter.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ConstraintView.hpp>
#include <Scenario/Document/Constraint/FullView/FullViewConstraintPresenter.hpp>
#include <Scenario/Document/DisplayedElements/DisplayedElementsProviderList.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Event/EventPresenter.hpp>
#include <Scenario/Document/Event/EventView.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/State/StatePresenter.hpp>
#include <Scenario/Document/State/StateView.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodePresenter.hpp>
#include <Scenario/Document/TimeNode/TimeNodeView.hpp>
#include <iscore/model/EntityMap.hpp>
#include <iscore/model/IdentifiedObjectMap.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore/tools/Todo.hpp>
#include <iscore/widgets/GraphicsProxyObject.hpp>
#include <tuple>
#include <type_traits>

namespace Scenario
{
class DisplayedElementsModel;

DisplayedElementsPresenter::DisplayedElementsPresenter(
    ScenarioDocumentPresenter* parent)
    : QObject{parent}
    , BaseScenarioPresenter<DisplayedElementsModel, FullViewConstraintPresenter>{
        parent->displayedElements}
    , m_model{parent}
{
}

DisplayedElementsPresenter::~DisplayedElementsPresenter()
{
  disconnect(
      &m_model->context().updateTimer, &QTimer::timeout, this,
      &DisplayedElementsPresenter::on_constraintExecutionTimer);

  // TODO use directly displayedelementspresentercontainer
  delete m_constraintPresenter;
  delete m_startStatePresenter;
  delete m_endStatePresenter;
  delete m_startEventPresenter;
  delete m_endEventPresenter;
  delete m_startNodePresenter;
  delete m_endNodePresenter;
}

BaseGraphicsObject& DisplayedElementsPresenter::view() const
{
  return *m_model->view().baseItem();
}

void DisplayedElementsPresenter::on_displayedConstraintChanged(
    const ConstraintModel& m)
{
  disconnect(
      &m_model->context().updateTimer, &QTimer::timeout, this,
      &DisplayedElementsPresenter::on_constraintExecutionTimer);

  for (auto& con : m_connections)
    QObject::disconnect(con);

  m_connections.clear();
  // TODO use directly displayedelementspresentercontainer
  delete m_constraintPresenter;
  delete m_startStatePresenter;
  delete m_endStatePresenter;
  delete m_startEventPresenter;
  delete m_endEventPresenter;
  delete m_startNodePresenter;
  delete m_endNodePresenter;

  // Create states / events
  auto& ctx = m_model->context();
  auto& provider = ctx.app.interfaces<DisplayedElementsProviderList>();
  DisplayedElementsPresenterContainer elts = provider.make(
      &DisplayedElementsProvider::make_presenters, m, ctx,
      m_model->view().baseItem(), this);
  m_constraintPresenter = elts.constraint;
  m_startStatePresenter = elts.startState;
  m_endStatePresenter = elts.endState;
  m_startEventPresenter = elts.startEvent;
  m_endEventPresenter = elts.endEvent;
  m_startNodePresenter = elts.startNode;
  m_endNodePresenter = elts.endNode;

  m_connections.push_back(
      con(m_constraintPresenter->model().duration,
          &ConstraintDurations::defaultDurationChanged, this,
          &DisplayedElementsPresenter::on_displayedConstraintDurationChanged));

  m_connections.push_back(connect(
      m_constraintPresenter, &FullViewConstraintPresenter::heightChanged, this,
      [&]() {
        on_displayedConstraintHeightChanged(
            m_constraintPresenter->view()->height());
      }));

  auto elements = std::make_tuple(
      m_constraintPresenter,
      m_startStatePresenter,
      m_endStatePresenter,
      m_startEventPresenter,
      m_endEventPresenter,
      m_startNodePresenter,
      m_endNodePresenter);

  ossia::for_each_in_tuple(elements, [&](auto elt) {
    using elt_t = std::remove_reference_t<decltype(*elt)>;
    m_connections.push_back(connect(
        elt, &elt_t::pressed, m_model, &ScenarioDocumentPresenter::pressed));
    m_connections.push_back(connect(
        elt, &elt_t::moved, m_model, &ScenarioDocumentPresenter::moved));
    m_connections.push_back(connect(
        elt, &elt_t::released, m_model, &ScenarioDocumentPresenter::released));
  });

  showConstraint();

  on_zoomRatioChanged(m_constraintPresenter->zoomRatio());

  con(ctx.updateTimer, &QTimer::timeout, this,
      &DisplayedElementsPresenter::on_constraintExecutionTimer);
}

void DisplayedElementsPresenter::showConstraint()
{
  // We set the focus on the main scenario.
  auto& rack = m_constraintPresenter->getSlots();
  if (!rack.empty())
  {
    emit requestFocusedPresenterChange(rack.front().process.presenter);
  }

  m_constraintPresenter->updateHeight();
}

void DisplayedElementsPresenter::on_zoomRatioChanged(ZoomRatio r)
{
  if (!m_constraintPresenter)
    return;
  updateLength(m_constraintPresenter->model()
                   .duration.defaultDuration()
                   .toPixels(r));

  m_constraintPresenter->on_zoomRatioChanged(r);
}

void DisplayedElementsPresenter::on_elementsScaleChanged(double s)
{
}

void DisplayedElementsPresenter::on_displayedConstraintDurationChanged(
    TimeVal t)
{
  updateLength(t.toPixels(m_model->zoomRatio()));
}

const double deltaX = 10.;
const double deltaY = 20.;
void DisplayedElementsPresenter::on_displayedConstraintHeightChanged(
    double size)
{
  m_model->updateRect({qreal(ScenarioLeftSpace), 0,
                       m_constraintPresenter->model()
                           .duration.defaultDuration()
                           .toPixels(m_constraintPresenter->zoomRatio()),
                       size});

  m_startEventPresenter->view()->setPos(deltaX, deltaY);
  m_startNodePresenter->view()->setPos(deltaX, deltaY);
  m_startStatePresenter->view()->setPos(deltaX, deltaY);
  m_constraintPresenter->view()->setPos(deltaX, deltaY);

  m_startEventPresenter->view()->setExtent({0., 0.});
  m_startNodePresenter->view()->setExtent({0., size});
  m_endEventPresenter->view()->setExtent({0., 1.});
  m_endNodePresenter->view()->setExtent({0., size});
}

void DisplayedElementsPresenter::updateLength(double length)
{
  m_endStatePresenter->view()->setPos({deltaX + length, deltaY});
  m_endEventPresenter->view()->setPos({deltaX + length, deltaY});
  m_endNodePresenter->view()->setPos({deltaX + length, deltaY});
}

void DisplayedElementsPresenter::on_constraintExecutionTimer()
{
  m_constraintPresenter->on_playPercentageChanged(
      m_constraintPresenter->model().duration.playPercentage());
}
}
