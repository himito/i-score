#pragma once
#include <QString>
#include <QWidget>

class QCheckBox;
class QPushButton;

namespace Scenario
{
// TODO refactor with SelectableButton
class EventShortCut final : public QWidget
{
  Q_OBJECT
public:
  EventShortCut(QString eventId, QWidget* parent = nullptr);

  bool isChecked();
  QString eventName();

signals:
  void eventSelected();

private:
  QCheckBox* m_box;
  QPushButton* m_eventBtn;
};
}
