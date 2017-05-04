#include <QLabel>
#include <QVBoxLayout>
#include <list>

#include "AutomationStateInspector.hpp"
#include <Automation/State/AutomationState.hpp>
#include <Inspector/InspectorWidgetBase.hpp>
#include <Process/State/ProcessStateDataInterface.hpp>
#include <State/Message.hpp>
#include <iscore/tools/Todo.hpp>

#include <iscore/widgets/TextLabel.hpp>

namespace Automation
{
StateInspectorWidget::StateInspectorWidget(
    const ProcessState& object,
    const iscore::DocumentContext& doc,
    QWidget* parent)
    : InspectorWidgetBase{object, doc, parent}
    , m_state{object}
    , m_label{new TextLabel}
{
  std::list<QWidget*> vec;
  vec.push_back(m_label);

  con(m_state, &ProcessStateDataInterface::stateChanged, this,
      &StateInspectorWidget::on_stateChanged);

  on_stateChanged();

  updateSectionsView(safe_cast<QVBoxLayout*>(layout()), vec);
}

void StateInspectorWidget::on_stateChanged()
{
  m_label->setText(m_state.message().toString());
}
}
