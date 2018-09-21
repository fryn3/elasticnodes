#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include <QString>
#include "nodeedgeparent.h"
class GraphWidget;
class Edge;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Node : public NodeEdgeParent
{
public:
    static const QPen _pen;
    Node(GraphWidget *graphWidget, QString textInNode = "");
    virtual ~Node() override;

    void setTextContent(QString text) override;
    QString textContent() const override;
    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
    void addEdge(Edge *edge);
    QList<Edge *> edges() const;
    enum { Type = NodeEdgeParent::Type + 1 };
    int type() const override { return Type; }
    enum { Radius = 40 };

    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    static int idStatic();
    void removeEdge(Edge *edge);
    int id() const override;
protected:
    int _id;
    QList<Edge *> edgeList;
    QPointF newPos;
    static int _idStatic;
    QString textInNode;  // который будет внутри вершины
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // NODE_H
