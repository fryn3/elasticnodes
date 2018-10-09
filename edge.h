#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include "nodeedgeparent.h"
#include "node.h"

class GraphWidget;

class EdgeParent : public NodeEdgeParent
{
protected:
    EdgeParent(Node *sourceNode, Node *destNode = nullptr, QString textArrow = "");
    EdgeParent(GraphWidget *graphWidget);
public:
    virtual ~EdgeParent() override;
    void setTextContent(QString text) override;
    QString textContent() const override;
    Node *sourceNode() const;
    Node *destNode() const;
    void adjust();
    enum { Type = NodeEdgeParent::Type + 2 };
    int type() const override { return Type; }
    QPainterPath pathPoint(QPointF point) const;
protected:
    QString textEdge;
    Node *source, *dest;
    qreal arrowSize;
    static int _idStatic;
    QPolygonF arrowPolygon(QPointF peak, qreal angle) const;
    virtual QPointF posText() const = 0;
};


class Edge : public EdgeParent
{
public:
    Edge(Node *sourceNode, Node *destNode = nullptr, QString textArrow = "");
    Edge(const QJsonObject &json, GraphWidget *graphWidget);

    enum { Type = EdgeParent::Type + 1 };
    int type() const override { return Type; }

    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
protected:
    QPointF bezier;         // для Кривой Безье
    QLineF beforeLine;      // от Источника до Получателя
    QRectF boundingRect() const override;   // Для отрисовки
    QPainterPath shape() const override;    // Для столкновений и выделения
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPointF newPosBezier() const;
    QPointF posText() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QPainterPath pathBezierCurve() const;
};

class EdgeCircle : public EdgeParent
{
public:
    EdgeCircle(Node *sourceNode, QString textArrow = "");
//    QPointF pointZero, pointZeroNode;
    enum { Type = EdgeParent::Type + 2 };
    int type() const override { return Type; }
protected:
    qreal radiusCircle;
    QRectF boundingRect() const override;   // Для отрисовки
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    inline QPointF centerPos() const { return QPointF(source->pos().x() + Node::Radius, source->pos().y() - Node::Radius); }
    inline QLineF lineCC() const { return QLineF(mapFromScene(source->pos()), centerPos()); } // растояние между центрами
    QPointF handlePos() const;
    bool handlePress(QPointF posPress);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    bool flHandlePress;
    QPainterPath shape() const override;
    inline bool correctMove() const {
        return !(lineCC().length() > Node::Radius + radiusCircle
                || lineCC().length() < std::abs(Node::Radius - radiusCircle));
    }
    QPointF peakArrow() const;
    inline qreal angleArrow() const { return QLineF(centerPos(), peakArrow()).angle() - 83; }
    QPointF posText() const override;
};

#endif // EDGE_H
