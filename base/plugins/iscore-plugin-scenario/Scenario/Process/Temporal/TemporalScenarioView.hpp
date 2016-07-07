#pragma once
#include <Process/LayerView.hpp>
#include <QPoint>
#include <QRect>

class QGraphicsItem;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneDragDropEvent;
class QGraphicsSceneMouseEvent;
class QKeyEvent;
class QMimeData;
class QPainter;

namespace Scenario
{
class TemporalScenarioPresenter;

class TemporalScenarioView final :
        public Process::LayerView
{
        Q_OBJECT

    public:
        TemporalScenarioView(QGraphicsItem* parent);
        ~TemporalScenarioView();

        void paint_impl(QPainter* painter) const override;

        void setSelectionArea(const QRectF& rect)
        {
            m_selectArea = rect;
            update();
        }

        void setPresenter(TemporalScenarioPresenter* pres)
        {
            m_pres = pres;
        }

    signals:
        void pressed(QPointF);
        void released(QPointF);
        void moved(QPointF);
        void doubleClick(QPointF);

        void clearPressed();
        void escPressed();

        void keyPressed(int);
        void keyReleased(int);

        // Screen pos, scene pos
        void askContextMenu(const QPoint&, const QPointF&);
        void dropReceived(const QPointF& pos, const QMimeData*);


    public slots:
        void lock()
        {
            m_lock = true;
            update();
        }
        void unlock()
        {
            m_lock = false;
            update();
        }

        void pressedAsked(const QPointF& p)
        {
            m_previousPoint = p;
            emit pressed(p);
        }
        void movedAsked(const QPointF& p);


    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
        void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

        void keyPressEvent(QKeyEvent *event) override;
        void keyReleaseEvent(QKeyEvent *event) override;

        void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    private:
        QRectF m_selectArea;

        bool m_lock {};
        TemporalScenarioPresenter* m_pres{};

        QPointF m_previousPoint{};
};
}
