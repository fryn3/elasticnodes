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
    : NodeEdgeParent(graphWidget), id(_idStatic++)//, graph(graphWidget)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(10);
    graph->scene()->addItem(this);    // сразу добавляет на сцену
    if (text.isEmpty()) {
        textInNode = QString("%1").arg(id);
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

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
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
