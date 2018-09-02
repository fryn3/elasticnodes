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
#ifdef DEBUG
    QPen pen2(Qt::yellow, 1, Qt::DotLine);
    painter->setOpacity(0.4);
    painter->setPen(pen2);
    painter->drawRect(boundingRect());
    painter->setOpacity(1);
    QPen pen(Qt::red, 1, Qt::SolidLine);
    painter->strokePath(shape(), pen);
#endif
}
