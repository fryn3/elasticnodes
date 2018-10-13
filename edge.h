#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include "nodeedgeparent.h"
#include "node.h"

class GraphWidget;

class EdgeParent : public NodeEdgeParent
{
protected:
    EdgeParent(Node *sourceNode, Node *destNode = nullptr, QString text = nullptr);
    EdgeParent(GraphWidget *graphWidget);
public:
    virtual ~EdgeParent() override;
    void setTextContent(QString text) override;
    inline Node *sourceNode() const { return source; }
    inline Node *destNode() const { return dest; }
    void adjust();
    enum { Type = NodeEdgeParent::Type + 2 };
    int type() const override { return Type; }
    QPainterPath pathPoint(QPointF point) const;
    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
    static EdgeParent *create(const QJsonObject &json, GraphWidget *graphWidget);
protected:
    Node *source, *dest;
    qreal arrowSize;
    static int _idStatic;
    QPolygonF arrowPolygon(QPointF peak, qreal angle) const;
    virtual QPointF posText() const = 0;
    QPainterPath pathText() const;
};


class Edge : public EdgeParent
{
public:
    Edge(Node *sourceNode, Node *destNode = nullptr, QString text = nullptr);
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
    EdgeCircle(Node *sourceNode, QString text = nullptr);
    enum { Type = EdgeParent::Type + 2 };
    int type() const override { return Type; }
    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
    EdgeCircle(const QJsonObject &json, GraphWidget *graphWidget);
protected:
    qreal radiusCircle;
    QRectF boundingRect() const override;   // Для отрисовки
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    inline QPointF centerPos() const { return QPointF(Node::Radius, -Node::Radius) + source->pos() + pos(); }
    inline QLineF lineCC() const { return QLineF(mapFromScene(source->pos()), mapFromScene(centerPos())); } //  линия между центрами
    inline bool correctMove() const {
        return !(lineCC().length() > Node::Radius + radiusCircle
                || lineCC().length() < std::abs(Node::Radius - radiusCircle));
    }
    inline qreal angleArrow() const { return QLineF(mapFromScene(centerPos()), peakArrow()).angle() - 83; }

    inline QPointF centerPos(QPointF newPos) const { return QPointF(Node::Radius, -Node::Radius) + source->pos() + newPos; }
    inline QLineF lineCC(QPointF newPos) const { return QLineF(mapFromScene(source->pos()), mapFromScene(centerPos(newPos))); } //  линия между центрами
    inline bool correctMove(QPointF newPos) const {
        return !(lineCC(newPos).length() > Node::Radius + radiusCircle
                || lineCC(newPos).length() < std::abs(Node::Radius - radiusCircle));
    }

    QPointF handlePos() const;
    bool handlePress(QPointF posPress);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    bool flHandlePress;
    QPainterPath shape() const override;
    QPointF peakArrow() const;
    QPointF posText() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // EDGE_H
