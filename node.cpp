#include "edge.h"
#include "node.h"
#include "graphwidget.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

const QPen Node::_pen = QPen(Qt::black, 2);
int Node::_idStatic = 0;

Node::Node(GraphWidget *graphWidget, QString text)
    : NodeEdgeParent(graphWidget, _idStatic++, text)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(10);
#ifdef DEBUG_NODE_EDGE
    setZValue(-10);
#endif
    graph->scene()->addItem(this);    // сразу добавляет на сцену
}

Node::~Node()
{
    foreach (EdgeParent *edge, edgeList)
        delete edge;
}

void Node::writeToJson(QJsonObject &json) const
{
    NodeEdgeParent::writeToJson(json);
    QJsonObject jsonPos
    {
        {"x", pos().x()},
        {"y", pos().y()},
    };
    QJsonObject jsonIt = json["Item"].toObject();
    jsonIt["pos"] = jsonPos;
    json["Item"] = jsonIt;
}

void Node::readFromJson(const QJsonObject &json)
{
    NodeEdgeParent::readFromJson(json);
    if (missKey(json, "Item")) {
        return;
    }
    QJsonObject jsonIt = json["Item"].toObject();
    if (_idStatic <= _id) {
        _idStatic = _id + 1;
    }
    if (missKey(jsonIt, "pos")) {
        return;
    }
    QJsonObject jsonPos = jsonIt["pos"].toObject();
    if (missKeys(jsonPos, QStringList { "x", "y" })) {
        return;
    }
    double x = jsonPos["x"].toDouble();
    double y = jsonPos["y"].toDouble();
    setPos(x, y);
}

void Node::addEdge(EdgeParent *edge)
{
    if (!edgeList.contains(edge))
        edgeList << edge;
}

QList<EdgeParent *> Node::edges() const
{
    return edgeList;
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-Radius - _pen.width() / 2,
                    -Radius - _pen.width() / 2,
                    2 * Radius + _pen.width(),
                    2 * Radius + _pen.width());
    return path;
}

QRectF Node::boundingRect() const
{
    return shape().boundingRect();
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    NodeEdgeParent::paint(painter, option, widget);
    painter->setBrush((option->state & QStyle::State_Selected ? Qt::cyan: Qt::white));
    painter->setPen(_pen);
    painter->drawEllipse(-Radius, -Radius, 2 * Radius, 2 * Radius);
    painter->setFont(QFont("Times", 12, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignCenter, _textContent);
    // Draw the node name
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10)); 
    QRectF textRect = boundingRect().adjusted(0, Radius / 2, 0, 0); 
    painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, getName()); 
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (EdgeParent *edge, edgeList) {
            edge->adjust();
        }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

int Node::idStatic()
{
    return _idStatic;
}

void Node::removeEdge(EdgeParent *edge)
{
    QMutableListIterator<EdgeParent *> iter(edgeList);
    while (iter.hasNext()) {
        EdgeParent *e = iter.next();
        if (e == edge) {
            iter.remove();
            break;
        }
    }
}
QString Node::getName() const
{
    return _name;
}
void Node::setName(const QString& name)
{
    _name = name;
}
