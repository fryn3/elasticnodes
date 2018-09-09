#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include "nodeedgeparent.h"

class GraphWidget;
class Node;

class Edge : public NodeEdgeParent
{
public:
    Edge(Node *sourceNode, Node *destNode, QString textArrow = "");
    virtual ~Edge() override;
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
    void bezierPosFinded();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QPainterPath pathBezierCurve() const;
    QPainterPath pathBezierPoint() const;
private:
    static uint _idStatic;
    static int _offsAngle;
    Node *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    QPointF bezier;         // для Кривой Безье
    qreal angleBezier;      // угол от Источника
    qreal distBezier;       // расстояние до Источника
    QLineF beforeLine;      // от Источника до Получателя
    QPointF textPoint;
    qreal arrowSize;
    bool flSelected;
};

#endif // EDGE_H
