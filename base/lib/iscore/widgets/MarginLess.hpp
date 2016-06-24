#pragma once

#include <QWidget>

namespace iscore
{
/**
 * @brief The MarginLess class
 *
 * A mixin that removes the margin of a layout.
 */
template<typename Layout>
class MarginLess final : public Layout
{
    public:
        MarginLess(QWidget* widg):
            Layout{widg}
        {
            this->setContentsMargins(0, 0, 0, 0);
            this->setSpacing(0);
            widg->setLayout(this);
        }
        MarginLess()
        {
            this->setContentsMargins(0, 0, 0, 0);
            this->setSpacing(0);
        }
};
}
