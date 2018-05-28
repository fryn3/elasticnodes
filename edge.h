#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include "nodeedgeparent.h"
class Node;

class Edge : public NodeEdgeParent
{
public:
    Edge(Node *sourceNode, Node *destNode, QString textArrow = "");
    virtual ~Edge();
    void setTextContent(QString text) override;
    QString textContent() const override;
    const uint id;
    Node *sourceNode() const;
    Node *destNode() const;

    void adjust();

    enum { Type = NodeEdgeParent::Type + 2 };
    int type() const override { return Type; }

protected:
    QString textEdge;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    Node *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    QPointF textPoint;
    qreal arrowSize;
    static uint _idStatic;
};

#endif // EDGE_H
