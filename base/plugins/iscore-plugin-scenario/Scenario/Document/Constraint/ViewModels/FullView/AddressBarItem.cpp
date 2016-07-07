#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <QtAlgorithms>
#include <QMap>
#include <QObject>
#include <QString>
#include <algorithm>
#include <cstddef>
#include <vector>

#include "AddressBarItem.hpp"
#include "ClickableLabelItem.hpp"
#include <Process/ModelMetadata.hpp>
#include <iscore/tools/ObjectIdentifier.hpp>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
AddressBarItem::AddressBarItem(QGraphicsItem *parent):
    QGraphicsObject{parent}
{
    this->setFlag(QGraphicsItem::ItemHasNoContents, true);
}

void AddressBarItem::setTargetObject(ObjectPath && path)
{
    qDeleteAll(m_items);
    m_items.clear();

    m_currentPath = std::move(path);

    double currentWidth = 0;

    int i = -1;
    for(auto& identifier : m_currentPath)
    {
        i++;
        if(!identifier.objectName().contains("ConstraintModel"))
            continue;

        auto thisPath = m_currentPath;
        auto& pathVec = thisPath.vec();
        pathVec.resize(i + 1);
        ConstraintModel& thisObj = thisPath.find<ConstraintModel>();

        QString txt = thisObj.metadata.name();

        auto lab = new ClickableLabelItem{
                   thisObj.metadata,
                [&] (ClickableLabelItem* item) {
                    emit constraintSelected(thisObj);
                },
                txt,
                this};

        lab->setIndex(i);
        connect(lab, &ClickableLabelItem::textChanged,
               this, &AddressBarItem::redraw);

        m_items.append(lab);
        lab->setPos(currentWidth, 0);
        currentWidth += 10 + lab->boundingRect().width();

        auto sep = new SeparatorItem{this};
        sep->setPos(currentWidth, 0);
        currentWidth += 10 + sep->boundingRect().width();
        m_items.append(sep);
    }

    prepareGeometryChange();
    m_width = currentWidth;
}


double AddressBarItem::width() const
{
    return m_width;
}



QRectF AddressBarItem::boundingRect() const
{
    return {};
}

void AddressBarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}

void AddressBarItem::redraw()
{
    double currentWidth = 0;
    for(auto obj : m_items)
    {
        obj->setPos(currentWidth, 0);
        currentWidth += 10 + obj->boundingRect().width();
    }

    emit needRedraw();
}
}
