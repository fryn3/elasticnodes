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
    Node *sourceNode() const;
    Node *destNode() const;
    void adjust();

    enum { Type = NodeEdgeParent::Type + 2 };
    int type() const override { return Type; }
    int id() const override;

    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
protected:
    int _id;
    QString textEdge;
    Node *source, *dest;
    QPointF bezier;         // для Кривой Безье
    QLineF beforeLine;      // от Источника до Получателя
    qreal arrowSize;
    bool flSelected;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPointF newPosBezier() const;
    QPointF newPosText() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QPainterPath pathBezierCurve() const;
    QPainterPath pathBezierPoint() const;
    static int _idStatic;
};

#endif // EDGE_H
