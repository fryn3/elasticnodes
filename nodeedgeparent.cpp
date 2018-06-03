#include "nodeedgeparent.h"
#include <QDebug>
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
