#pragma once
#include <QWidget>
#include <QGridLayout>
#include <iscore/widgets/MarginLess.hpp>
#include <iscore/widgets/ClearLayout.hpp>

/**
 * @brief The WidgetWrapper class
 *
 * This wrapper is used to easily wrap a ValueWidget
 * so that the ValueWidget can be replaced without clearing all the layout.
 */
template<typename Widget>
class WidgetWrapper final : public QWidget
{
    public:
        explicit WidgetWrapper(QWidget* parent):
            QWidget{parent}
        {
            m_lay = new iscore::MarginLess<QGridLayout>{this};
        }

        void setWidget(Widget* widg)
        {
            iscore::clearLayout(m_lay);
            m_widget = widg;

            if(m_widget)
                m_lay->addWidget(m_widget);
        }

        Widget* widget() const
        {
            return m_widget;
        }

    private:
        QGridLayout* m_lay{};
        Widget* m_widget{};
};
