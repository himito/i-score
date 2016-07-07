#include <Process/Style/ProcessFonts.hpp>
#include <Process/Style/Skin.hpp>
#include <QFlags>
#include <QFont>
#include <QGraphicsItem>
#include <qnamespace.h>
#include <QPainter>
#include <QRect>
#include <QGraphicsSceneMouseEvent>

#include "AutomationView.hpp"
#include <Process/LayerView.hpp>

const int fontSize = 8;
namespace Automation
{
LayerView::LayerView(QGraphicsItem* parent) :
    Process::LayerView {parent}
{
    setZValue(1);
    setFlags(ItemClipsChildrenToShape | ItemIsSelectable | ItemIsFocusable);
    setAcceptDrops(true);
    auto f = Skin::instance().SansFont;
    f.setPointSize(fontSize);

    m_textcache.setFont(f);
    m_textcache.setCacheEnabled(true);
}

LayerView::~LayerView()
{

}

void LayerView::setDisplayedName(const QString& s)
{
    m_displayedName = s;

    m_textcache.setText(s);
    m_textcache.beginLayout();
    QTextLine line = m_textcache.createLine();
    line.setPosition(QPointF{0., 0.});

    m_textcache.endLayout();

    update();
}

void LayerView::paint_impl(QPainter* painter) const
{
#if !defined(ISCORE_IEEE_SKIN)
    if(m_showName)
    {
        m_textcache.draw(painter, QPointF{5, fontSize});
    }
#endif
}

void LayerView::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    emit dropReceived(event->mimeData());
}
}
