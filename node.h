#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include <QString>
#include "nodeedgeparent.h"
class Edge;
class GraphWidget;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Node : public NodeEdgeParent
{
public:
    static const QPen _pen;
    Node(GraphWidget *graphWidget, QString textInNode = "");
    virtual ~Node();
    const uint id;
    void setTextContent(QString text) override;
    QString textContent() const override;
    void addEdge(Edge *edge);
    QList<Edge *> edges() const;
    enum { Type = NodeEdgeParent::Type + 1 };
    int type() const override { return Type; }
    enum { Radius = 40 };

    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    static uint idStatic();
    void removeEdge(Edge *edge);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QString textInNode;  // который будет внутри вершины

private:
    QList<Edge *> edgeList;
    QPointF newPos;
    GraphWidget *graph;
    static uint _idStatic;
};

#endif // NODE_H
