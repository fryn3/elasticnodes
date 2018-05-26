#include "edge.h"
#include "node.h"

#include <qmath.h>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

uint Edge::_idStatic = 0;

Edge::Edge(Node *sourceNode, Node *destNode)
    : id(_idStatic++), arrowSize(15)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    if(source != dest)
        dest->addEdge(this);
    adjust();
}

Edge::~Edge()
{
    source->removeEdge(this);
    if (source != dest)
        dest->removeEdge(this);
}

Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    if(source != dest) {
        QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
        qreal length = line.length();

        prepareGeometryChange();
        if (length > qreal(2 * Node::Radius)) {
            QPointF edgeOffset((line.dx() * Node::Radius) / length, (line.dy() * Node::Radius) / length);
            sourcePoint = line.p1() + edgeOffset;
            destPoint = line.p2() - edgeOffset;
            // Нахождение точки для текста
            QLineF line1(sourcePoint, destPoint), line2;
            if (line1.angle() >= 0 && line1.angle() <= 180) {
                line1.setLength(line1.length() - 40);
                line2.setPoints(line1.p2(), line1.p1());
            } else {
                QLineF line11(destPoint, sourcePoint);
                line11.setLength(40);
                line2.setPoints(line11.p2(), line11.p1());
            }
            QLineF line3 = line2.normalVector();
            line3.setLength(14);
            textPoint = line3.p2();
        } else {
            sourcePoint = destPoint = line.p1();
        }

    } else {
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
        prepareGeometryChange();
        sourcePoint = mapFromItem(source, 0, -Node::Radius);
        destPoint = mapFromItem(source, Node::Radius, 0);
    }
}

// Для столкновений и выделения
QPainterPath Edge::shape() const{
    QPainterPath path;
    if (source != dest) {
        QLineF line = QLineF(sourcePoint.x(), sourcePoint.y(), destPoint.x(), destPoint.y());
        qreal radAngle = line.angle() * M_PI / 180;
        qreal selectionOffset = 3;
        qreal dx = selectionOffset * sin(radAngle);
        qreal dy = selectionOffset * cos(radAngle);
        QPointF offset1 = QPointF(dx, dy);
        path.moveTo(line.p1() + offset1);
        path.lineTo(line.p1() - offset1);
        path.lineTo(line.p2() - offset1);
        path.lineTo(line.p2() + offset1);
    } else {
        path.addEllipse(source->pos() + QPointF(Node::Radius, -Node::Radius),
                        Node::Radius + 2, Node::Radius + 2);
    }
    return path;
}

// Для отрисовки
QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();
    if (source != dest) {
        QPolygonF pText;
        qreal x = textPoint.x();
        qreal y = textPoint.y();
        pText << QPointF(x, y)
              << QPointF(x, y - 110)
              << QPointF(x + 70, y - 110)
              << QPointF(x + 70, y + 10)
              << QPointF(x, y + 10);
        return shape().boundingRect().united(pText.boundingRect());
    }
    return shape().boundingRect();
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (!source || !dest)
        return;
    double angle;
    QPointF peak, destArrowP1, destArrowP2;
    painter->setPen(QPen((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (source != dest) {
        QLineF line(sourcePoint, destPoint);
        if (qFuzzyCompare(line.length(), qreal(0.)))
            return;

        // Draw the line itself
        painter->drawLine(line);
        // Draw the arrows
        angle = std::atan2(-line.dy(), line.dx());

        destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI / 1.8) * qMin(arrowSize, line.length()));
        destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()));
        peak = line.p2();
    } else {
        painter->drawArc(source->pos().x(),
                         source->pos().y() - 2 * Node::Radius,
                         2 * Node::Radius,
                         2 * Node::Radius,
                         16 * -70, 16 * 270);
        angle = 1.07*M_PI;
        destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 1.8) * arrowSize,
                                                 cos(angle - M_PI / 1.8) * arrowSize);
        destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 1.8)* arrowSize,
                                                 cos(angle - M_PI + M_PI / 1.8) * arrowSize);
        painter->setBrush((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black));
        peak = destPoint;

    }
    painter->setBrush((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black));
    painter->drawPolygon(QPolygonF() << peak << destArrowP1 << destArrowP2);
    painter->setFont(QFont("Times", 11));
    painter->drawText(textPoint, "x2 y2");

//    painter->drawText(-Radius / 5, 0, QString("%1").arg(id));
//    painter->drawText(-Radius / 5, 0 + 10, QString("%1").arg(id));
}
