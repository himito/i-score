#pragma once
#include <QPointF>
#include <QDebug>

namespace Scenario
{
/**
 * @brief The VerticalExtent struct
 *
 * Used for "vertical" elements,
 * like TimeNode and constraints.
 *
 * The value is currently in percentage.
 * TODO assess if it would be better to have it in absolute
 * instead.
 *
 */
struct VerticalExtent final : public QPointF
{
        Q_DECL_CONSTEXPR VerticalExtent() = default;
        Q_DECL_CONSTEXPR VerticalExtent(qreal x, qreal y): QPointF{x, y} {}
        Q_DECL_CONSTEXPR VerticalExtent(const VerticalExtent&) = default;
        Q_DECL_CONSTEXPR VerticalExtent(VerticalExtent&&) noexcept = default;
        Q_DECL_CONSTEXPR VerticalExtent(QPointF other): QPointF{other} {}
        VerticalExtent& operator =(VerticalExtent other) { static_cast<QPointF&>(*this) = other; return *this; }
        VerticalExtent& operator =(QPointF other) {  static_cast<QPointF&>(*this) = other; return *this;  }

        Q_DECL_CONSTEXPR VerticalExtent operator*(qreal other)
        {
            return VerticalExtent{top() * other, bottom() * other};
        }

        Q_DECL_CONSTEXPR double top() const    { return QPointF::x(); }
        Q_DECL_CONSTEXPR double bottom() const { return QPointF::y(); }
};

inline QDebug operator<< (QDebug d, const VerticalExtent& ve)
{
    d << static_cast<QPointF>(ve);
    return d;
}
}
