#include <QBoxLayout>
#include <QToolButton>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/tools/Todo.hpp>
#include <qnamespace.h>

#include "SelectionStackWidget.hpp"
#include <iscore/selection/Selection.hpp>
#include <iscore/widgets/MarginLess.hpp>

namespace InspectorPanel
{
SelectionStackWidget::SelectionStackWidget(
    iscore::SelectionStack& s, QWidget* parent)
    : QWidget{parent}, m_stack{s}
{
  m_prev = new QToolButton{this};
  m_prev->setArrowType(Qt::LeftArrow);
  m_prev->setEnabled(m_stack.canUnselect());

  m_next = new QToolButton{this};
  m_next->setArrowType(Qt::RightArrow);
  m_next->setEnabled(m_stack.canReselect());

  auto lay = new iscore::MarginLess<QHBoxLayout>{this};
  lay->setSizeConstraint(QLayout::SetMinimumSize);
  lay->addWidget(m_prev);
  lay->addWidget(m_next);
  setLayout(lay);

  connect(m_prev, &QToolButton::pressed, [&]() { m_stack.unselect(); });

  connect(m_next, &QToolButton::pressed, [&]() { m_stack.reselect(); });

  con(m_stack, &iscore::SelectionStack::currentSelectionChanged, this,
      &SelectionStackWidget::selectionChanged);
}

void SelectionStackWidget::selectionChanged(const Selection& s)
{
  m_prev->setEnabled(m_stack.canUnselect());
  m_next->setEnabled(m_stack.canReselect());
}
}
