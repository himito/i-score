#pragma once
#include <QWidget>
#include <State/Unit.hpp>
#include <iscore_lib_state_export.h>

class QComboBox;
class QHBoxLayout;
namespace State
{
class ISCORE_LIB_STATE_EXPORT UnitWidget : public QWidget
{
  Q_OBJECT
public:
  UnitWidget(const State::Unit& u, QWidget* parent);

  State::Unit unit() const;
  void setUnit(const State::Unit&);

signals:
  void unitChanged(const State::Unit&);

private:
  void on_dataspaceChanged(const State::Unit&);

  QHBoxLayout* m_layout{};
  QComboBox* m_dataspace{};
  QComboBox* m_unit{};
};
}
