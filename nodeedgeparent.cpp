#include "nodeedgeparent.h"
#include <QDebug>
#include <QPainter>
class GraphWidget;

NodeEdgeParent::NodeEdgeParent(GraphWidget *graphWidget): graph(graphWidget)
{

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
