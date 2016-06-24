#pragma once
#include <QChar>

#include <State/Value.hpp>
#include "ValueWidget.hpp"

class QLineEdit;
class QWidget;

class ISCORE_LIB_STATE_EXPORT CharValueWidget : public State::ValueWidget
{
    public:
        CharValueWidget(QChar value, QWidget* parent = nullptr);

        State::Value value() const override;

    private:
        QLineEdit* m_value;
};
