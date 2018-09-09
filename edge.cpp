#include "edge.h"
#include "node.h"
#include <QString>
#include <qmath.h>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

const qreal PI = atan(1) * 4;
uint Edge::_idStatic = 0;
int Edge::_offsAngle = 5;

Edge::Edge(Node *sourceNode, Node *destNode, QString textArrow)
    : NodeEdgeParent(sourceNode->graph),
      id(_idStatic++), textEdge(textArrow), arrowSize(15)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    if(source != dest)
        dest->addEdge(this);
    beforeLine.setPoints(source->pos(), dest->pos());   // от Источника до Получателя
    QLineF line1(source->pos(), dest->pos());
    line1.setLength(line1.length() / 2);
    QLineF line2(line1.p2(), line1.p1());
    QLineF line3 = line2.normalVector();
    line3.setLength(30);
    bezier.setX(line3.p2().x());
    bezier.setY(line3.p2().y());                        // bezier
    QLineF line4(source->pos(), bezier);
    distBezier = line4.length();                        // distBezier
    angleBezier = beforeLine.angle() - line4.angle();//line1.angleTo(line4);                 // angleBezier
    adjust();
}

Edge::~Edge()
{
    source->removeEdge(this);
    if (source != dest)
        dest->removeEdge(this);
}

void Edge::setTextContent(QString text)
{
    textEdge = text;
    adjust();
}

QString Edge::textContent() const
{
    return textEdge;
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
        bool offs = false;  // смещение с центра
        {
            QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
            qreal length = line.length();
            qreal angl = line.angle();
            for (auto edg : source->edges()) {
                if (edg->sourceNode() == dest) {
                    offs = true;
                    if(id > edg->id) {
                        edg->adjust();
                    }
                    break;
                }
            }
            prepareGeometryChange();
            if (!offs) {
                if (length > qreal(2 * Node::Radius)) {
                    QPointF edgeOffset((line.dx() * Node::Radius) / length, (line.dy() * Node::Radius) / length);
                    sourcePoint = line.p1() + edgeOffset;
                    destPoint = line.p2() - edgeOffset;
                } else {
                    sourcePoint = destPoint = line.p1();
                }
            } else {    // offs
                if (length > qreal(2 * Node::Radius)) {
                    sourcePoint.setX(line.p1().x() + Node::Radius * cos((angl + _offsAngle) * PI / 180));
                    sourcePoint.setY(line.p1().y() - Node::Radius * sin((angl + _offsAngle) * PI / 180));
                    destPoint.setX(line.p2().x() + Node::Radius * cos((angl - 180 - _offsAngle) * PI / 180));
                    destPoint.setY(line.p2().y() - Node::Radius * sin((angl - 180 - _offsAngle) * PI / 180));
                } else {
                    sourcePoint = destPoint = line.p1();
                }
            }
        }
        // Нахождение точки для текста
        // for QFont("Times", 11)
        QLineF line1(bezier, mapFromScene(dest->pos())), line2;
        line1.setLength(line1.length() - Node::Radius);
        qreal anglRad = line1.angle() * PI / 180;
        qDebug() << "line1(bezier, destPoint) = " << line1.angle();
        qreal widthText, hightText;
        widthText = 8 * textEdge.size();
        hightText = 14;
        qreal lenLine1 = line1.length();
        if (line1.angle() > 0 && line1.angle() <= 90) {
            // вдоль / поперек
            // widthText / hightText + 10   <= при 0
            // hightText / 10                <= при 90
            line1.setLength(line1.length() - (widthText * cos(anglRad) + hightText * sin(anglRad)));
            if (lenLine1 > (widthText * cos(anglRad) + hightText * sin(anglRad))) {
                line2.setPoints(line1.p2(), line1.p1());
            } else {
                line2.setPoints(line1.p1(), line1.p2());
            }
            QLineF line3 = line2.normalVector();
            line3.setLength(10 * sin(anglRad) + (hightText + 10) * cos(anglRad));
            textPoint = line3.p2();
        } else if (line1.angle() > 90 && line1.angle() <= 180) {
            // hightText / 10
            // 5 / 10
            line1.setLength(line1.length() - (5 * -cos(anglRad) + hightText * sin(anglRad)));
            if (lenLine1 > (5 * -cos(anglRad) + hightText * sin(anglRad))) {
                line2.setPoints(line1.p2(), line1.p1());
            } else {
                line2.setPoints(line1.p1(), line1.p2());
            }
            QLineF line3 = line2.normalVector();
            line3.setLength(10 * -cos(anglRad) + 10 * sin(anglRad));
            textPoint = line3.p2();
        } else if (line1.angle() > 180 && line1.angle() <= 270) {
            // 5 / hightText
            // 5 / 5
            QLineF line11(line1.p2(), bezier);
            line11.setLength(5);
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(hightText * -cos(anglRad) + 5 * -sin(anglRad));
            textPoint = line3.p2();
        } else {
            // 5 / 5
            // widthText / 5
            QLineF line11(line1.p2(), bezier);
            line11.setLength(5 * -sin(anglRad) + widthText * cos(anglRad));
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(5.0);
            textPoint = line3.p2();
        }
        qDebug() << textPoint;
    } else {        // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
        prepareGeometryChange();
        sourcePoint = mapFromItem(source, 0, -Node::Radius);
        destPoint = mapFromItem(source, Node::Radius, 0);
    }
    bezierPosFinded();
}

// Для столкновений и выделения
QPainterPath Edge::shape() const{
    QPainterPath path;
    if (source != dest) {
        QLineF line = QLineF(sourcePoint.x(), sourcePoint.y(), destPoint.x(), destPoint.y());
        qreal radAngle = line.angle() * M_PI / 180;
        qreal qOffset = 5;
        qreal dx = qOffset * sin(radAngle);
        qreal dy = qOffset * cos(radAngle);
        QPointF offset(dx, dy);
        path.moveTo(line.p1() + offset);
        path.lineTo(line.p1() - offset);
        path.lineTo(line.p2() - offset);
        path.lineTo(line.p2() + offset);
        path.lineTo(line.p1() + offset);

        QLineF line0(mapFromScene(source->pos()), bezier);
        QLineF line1(bezier, mapFromScene(dest->pos()));
        qreal dx0 = qOffset * sin(line0.angle() * M_PI / 180);
        qreal dy0 = qOffset * cos(line0.angle() * M_PI / 180);
        qreal dx1 = qOffset * sin(line1.angle() * M_PI / 180);
        qreal dy1 = qOffset * cos(line1.angle() * M_PI / 180);
        QPointF offset0(dx0, dy0);
        QPointF offset1(dx1, dy1);
        path.moveTo(mapFromScene(source->pos() + offset0));
        path.cubicTo(bezier + (offset0 + offset1) / 2, bezier + (offset0 + offset1) / 2, mapFromScene(dest->pos()) + offset1);
        path.lineTo(mapFromScene(dest->pos()) - offset1);
        path.cubicTo(bezier - (offset0 + offset1) / 2, bezier - (offset0 + offset1) / 2, mapFromScene(source->pos()) - offset0);
        path.lineTo(mapFromScene(source->pos()) + offset0);

        path.moveTo(bezier + QPointF(-3, -3));
        path.lineTo(bezier + QPointF(3, -3));
        path.lineTo(bezier + QPointF(3, 3));
        path.lineTo(bezier + QPointF(-3, 3));
        path.lineTo(bezier + QPointF(-3, -3));
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
              << QPointF(x, y - 18) // for QFont("Times", 11)
              << QPointF(x + 8 * textEdge.size(), y - 18)
              << QPointF(x + 8 * textEdge.size(), y + 2)
              << QPointF(x, y + 2);
        QPolygonF pBezier;
        pBezier << bezier + QPointF(-1, -1)
                << bezier + QPointF(2, 0)
                << bezier + QPointF(0, 2)
                << bezier + QPointF(-2, 0);
        return shape().boundingRect().united(pText.boundingRect()).united(pBezier.boundingRect());
    }
    return shape().boundingRect();
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!source || !dest)
        return;
    double angle;
    QPointF peak, destArrowP1, destArrowP2;
    painter->setPen(QPen((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (source != dest) {
        QPainterPath myPath;
        myPath.moveTo(mapFromScene(source->pos()));
        myPath.cubicTo(bezier, bezier, mapFromScene(dest->pos()));

        painter->drawPath(myPath);
        QLineF line(bezier, mapFromScene(dest->pos()));
        line.setLength(line.length() - Node::Radius);
        if (qFuzzyCompare(line.length(), qreal(0.)))
            return;

        // Draw the line itself
//        painter->drawLine(line);
        // Draw the arrows
        angle = std::atan2(-line.dy(), line.dx());

        destArrowP1 = line.p2() + QPointF(sin(angle - M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI / 1.8) * qMin(arrowSize, line.length()));
        destArrowP2 = line.p2() + QPointF(sin(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()));
        peak = line.p2();
    } else {
        painter->drawArc(static_cast<int>(source->pos().x()),
                         static_cast<int>(source->pos().y()) - 2 * Node::Radius,
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
    painter->drawText(textPoint, textEdge);
    painter->drawEllipse(bezier, 2, 2);         // размер точки
    NodeEdgeParent::paint(painter, option, widget);
}

void Edge::bezierPosFinded()
{
        QLineF line1(mapFromScene(source->pos()), mapFromScene(dest->pos()));
        qreal k = line1.length() / beforeLine.length();
        QLineF line2 = line1;
        line2.setAngle(line1.angle() - angleBezier);
        line2.setLength(k * distBezier);
        bezier.setX(line2.p2().x());
        bezier.setY(line2.p2().y());
}

QVariant Edge::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
    {
        beforeLine.setPoints(source->pos(), dest->pos());
        QLineF line4(source->pos(), mapToScene(bezier));
        distBezier = line4.length();                        // distBezier
        angleBezier = beforeLine.angle() - line4.angle();                 // angleBezier
        adjust();
     }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
