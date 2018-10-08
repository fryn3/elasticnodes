#include "nodeedgeparent.h"
#include <QDebug>
#include <QPainter>
class GraphWidget;

NodeEdgeParent::NodeEdgeParent(GraphWidget *graphWidget)
    : graph(graphWidget), _id(0)
{

}

int NodeEdgeParent::id() const
{
    return _id;
}

void NodeEdgeParent::writeToJson(QJsonObject &json) const
{
    json["Type"] = type();
}

void NodeEdgeParent::readFromJson(const QJsonObject &json)
{
    if (missKey(json, "Type")) {
        return;
    }
}

void NodeEdgeParent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        qDebug() << "RightButton";
        scene()->clearSelection();
        setSelected(true);
//        emit graph->editItem();
    }
    QGraphicsItem::mousePressEvent(event);
}

void NodeEdgeParent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef DEBUG_NODE_EDGE
    QPen pen2(Qt::yellow, 0, Qt::DotLine);
    painter->setOpacity(0.4);
    painter->setPen(pen2);
    painter->drawRect(boundingRect());
    painter->setOpacity(1);
    QPen pen(Qt::red, 0, Qt::SolidLine);
    painter->strokePath(shape(), pen);
#else
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
#endif
}

QRectF NodeEdgeParent::enoughBoundingRect(QRectF rect) const
{
    const qreal enough = 50;
    rect.moveCenter(rect.center() - QPointF(enough, enough));
    rect.setSize(rect.size() + QSizeF(2 * enough, 2 * enough));
    return rect;
}
