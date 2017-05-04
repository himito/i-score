#pragma once
#include <QAction>
#include <QInputDialog>
#include <QMenu>
#include <QObject>
#include <State/Expression.hpp>

namespace Scenario
{
// TODO use me with condition also.
class ExpressionMenu : public QObject
{
  Q_OBJECT
public:
  // Fun should be a function that returns an expression
  template <typename Fun>
  ExpressionMenu(Fun f, QWidget* parent)
      : menu{new QMenu{parent}}
      , deleteAction{menu->addAction(QObject::tr("Delete"))}
      , editAction{menu->addAction(QObject::tr("Edit"))}
      , addSubAction{menu->addAction(QObject::tr("Add sub-expression"))}
  {
    connect(editAction, &QAction::triggered, this, [=] {
      bool ok = false;
      auto str = QInputDialog::getText(
          nullptr,
          tr("Edit expression"),
          tr("Edit expression"),
          QLineEdit::Normal,
          f().toString(),
          &ok);
      if (ok)
        emit expressionChanged(std::move(str));
    });
  }

  QMenu* menu{};
  QAction* deleteAction{};
  QAction* editAction{};
  QAction* addSubAction{};

signals:
  void expressionChanged(QString);
};
}
