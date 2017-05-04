#include <QGraphicsItem>
#include <QMimeData>
#include <QRect>
#include <QSize>
#include <QStringList>
#include <Scenario/Commands/Event/State/AddStateWithData.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Event/EventView.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <State/MessageListSerialization.hpp>
#include <algorithm>
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/widgets/GraphicsItem.hpp>

#include "EventPresenter.hpp"
#include <Scenario/Process/ScenarioInterface.hpp>
#include <State/Expression.hpp>
#include <State/Message.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/serialization/MimeVisitor.hpp>

#include <iscore/tools/Todo.hpp>

class QObject;
#include <iscore/model/Identifier.hpp>

namespace Scenario
{
EventPresenter::EventPresenter(
    const EventModel& model, QGraphicsItem* parentview, QObject* parent)
    : QObject{parent}
    , m_model{model}
    , m_view{new EventView{*this, parentview}}
    , m_dispatcher{iscore::IDocument::documentContext(m_model).commandStack}
{
  // The scenario catches this :
  con(m_model.selection, &Selectable::changed, m_view,
      &EventView::setSelected);

  con(m_model.metadata(), &iscore::ModelMetadata::ColorChanged, m_view,
      &EventView::changeColor);

  con(m_model.metadata(), &iscore::ModelMetadata::CommentChanged, m_view,
      &EventView::changeToolTip);

  con(m_model, &EventModel::statusChanged, m_view, &EventView::setStatus);

  connect(
      m_view, &EventView::eventHoverEnter, this,
      &EventPresenter::eventHoverEnter);

  connect(
      m_view, &EventView::eventHoverLeave, this,
      &EventPresenter::eventHoverLeave);

  connect(m_view, &EventView::dropReceived, this, &EventPresenter::handleDrop);

  m_view->setCondition(m_model.condition().toString());
  m_view->setToolTip(m_model.metadata().getComment());

  con(m_model, &EventModel::conditionChanged, this,
      [&](const State::Expression& c) { m_view->setCondition(c.toString()); });
}

EventPresenter::~EventPresenter()
{
}

const Id<EventModel>& EventPresenter::id() const
{
  return m_model.id();
}

EventView* EventPresenter::view() const
{
  return m_view;
}

const EventModel& EventPresenter::model() const
{
  return m_model;
}

bool EventPresenter::isSelected() const
{
  return m_model.selection.get();
}

void EventPresenter::handleDrop(const QPointF& pos, const QMimeData* mime)
{
  // We don't want to create a new state in BaseScenario
  auto scenar = dynamic_cast<Scenario::ProcessModel*>(m_model.parent());
  // todo Maybe the drop should be handled by the scenario presenter ?? or not

  // If the mime data has states in it we can handle it.
  if (scenar && mime->formats().contains(iscore::mime::messagelist()))
  {
    Mime<State::MessageList>::Deserializer des{*mime};
    State::MessageList ml = des.deserialize();

    auto cmd = new Scenario::Command::AddStateWithData{
        *scenar, m_model.id(),
        pos.y() / m_view->parentItem()->boundingRect().size().height(),
        std::move(ml)};
    m_dispatcher.submitCommand(cmd);
  }
}
}
