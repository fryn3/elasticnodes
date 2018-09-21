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
    : NodeEdgeParent(graphWidget), _id(_idStatic++)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(10);
#ifdef DEBUG
    setZValue(-10);
#endif
    graph->scene()->addItem(this);    // сразу добавляет на сцену
    if (text.isEmpty()) {
        textInNode = QString("%1").arg(_id);
    } else {
        textInNode = text;
    }
}

Node::~Node()
{
    foreach (Edge *edge, edgeList)
        delete edge;
}

void Node::setTextContent(QString text)
{
    textInNode = text;
    update();
}

QString Node::textContent() const
{
    return textInNode;
}

void Node::writeToJson(QJsonObject &json) const
{
    QJsonObject jsonNode;
    jsonNode["id"] = _id;
    QJsonObject jsonPos
    {
        {"x", pos().x()},
        {"y", pos().y()},
    };
    jsonNode["pos"] = jsonPos;
    jsonNode["textContent"] = textContent();

    json["Node"] = jsonNode;
}

void Node::readFromJson(const QJsonObject &json)
{
    if (missKey(json, "Node")) {
        return;
    }
    QJsonObject jsonN = json["Node"].toObject();
    if (missKey(jsonN, "id")) {
        return;
    }
    _id = jsonN["id"].toInt();
    if (_idStatic <= _id) {
        _idStatic = _id + 1;
    }
    if (missKey(jsonN, "pos")) {
        return;
    }
    QJsonObject jsonPos = jsonN["pos"].toObject();
    if (missKey(jsonPos, "x") || missKey(jsonPos, "y")) {
        return;
    }
    double x = jsonPos["x"].toDouble();
    double y = jsonPos["y"].toDouble();
    setPos(x, y);
    if (missKey(jsonN, "textContent")) {
        return;
    }
    setTextContent(jsonN["textContent"].toString());
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
}

QList<Edge *> Node::edges() const
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
    painter->drawText(boundingRect(), Qt::AlignCenter, textInNode);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, edgeList)
            edge->adjust();
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

void Node::removeEdge(Edge *edge)
{
    QMutableListIterator<Edge *> iter(edgeList);
    while (iter.hasNext()) {
        Edge *e = iter.next();
        if (e == edge) {
            iter.remove();
            break;
        }
    }
}

int Node::id() const
{
    return _id;
}
