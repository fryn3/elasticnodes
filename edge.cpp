#include "edge.h"
#include "node.h"

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
    setFlag(QGraphicsItem::ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    if(source != dest)
        dest->addEdge(this);

    bezier.setX((source->pos().x()+dest->pos().x())/2);
    bezier.setY((source->pos().y()+dest->pos().y())/2);

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
        QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
        qreal length = line.length();
        qreal angl = line.angle();
        qreal anglRad = angl * PI / 180;
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
        //sourcePoint=line.p1();
        //destPoint=line.p2();

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

        // Нахождение точки для текста
        // for QFont("Times", 11)
        QLineF line1(sourcePoint, destPoint), line2;
        qreal widthText, hightText;
        widthText = 8 * textEdge.size();
        hightText = 14;
        if (line1.angle() > 0 && line1.angle() <= 90) {
            // вдоль / поперек
            // widthText / hightText + 10   <= при 0
            // hightText / 10                <= при 90
            line1.setLength(line1.length() - (widthText * cos(anglRad) + hightText * sin(anglRad)));
            line2.setPoints(line1.p2(), line1.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(10 * sin(anglRad) + (hightText + 10) * cos(anglRad));
            textPoint = line3.p2();
        } else if (line1.angle() > 90 && line1.angle() <= 180) {
            // hightText / 10
            // 5 / 10
            line1.setLength(line1.length() - (5 * -cos(anglRad) + hightText * sin(anglRad)));
            line2.setPoints(line1.p2(), line1.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(10 * -cos(anglRad) + 10 * sin(anglRad));
            textPoint = line3.p2();
        } else if (line1.angle() > 180 && line1.angle() <= 270) {
            // 5 / hightText
            // 5 / 5
            QLineF line11(destPoint, sourcePoint);
            line11.setLength(5);
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(hightText * -cos(anglRad) + 5 * -sin(anglRad));
            textPoint = line3.p2();
        } else {
            // 5 / 5
            // widthText / 5
            QLineF line11(destPoint, sourcePoint);
            line11.setLength(5 * -sin(anglRad) + widthText * cos(anglRad));
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(5.0);
            textPoint = line3.p2();
        }
    } else {        // source == dest
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
        /*
        QLineF line = QLineF(sourcePoint.x(), sourcePoint.y(), destPoint.x(), destPoint.y());
        qreal radAngle = line.angle() * M_PI / 180;
        qreal selectionOffset = 4;
        qreal dx = selectionOffset * sin(radAngle);
        qreal dy = selectionOffset * cos(radAngle);
        QPointF offset1 = QPointF(dx, dy);
        path.moveTo(line.p1() + offset1);
        path.lineTo(line.p1() - offset1);
        path.lineTo(line.p2() - offset1);
        path.lineTo(line.p2() + offset1);
        */


        path.moveTo(sourcePoint);
        QPointF ep(destPoint.x(), destPoint.y());
        path.cubicTo(bezier, bezier,ep);

        /*
        QPointF offset1 = QPointF(1, 1);
        path.moveTo(sourcePoint+offset1);
        path.quadTo(bezier+offset1,ep+offset1);
        path.moveTo(sourcePoint-offset1);
        path.quadTo(bezier-offset1,ep-offset1);
        offset1 = QPointF(2, 2);
        path.moveTo(sourcePoint+offset1);
        path.quadTo(bezier+offset1,ep+offset1);
        path.moveTo(sourcePoint-offset1);
        path.quadTo(bezier-offset1,ep-offset1);
        */
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
        //painter->drawLine(line);
        /*
        painter->drawArc(source->pos().x()-Node::Radius,
                         source->pos().y()-Node::Radius,
                         -(source->pos().x()-dest->pos().x())+Node::Radius,
                         -(source->pos().y()-dest->pos().y())+Node::Radius,
                         16 *0 , 16 *180);
        */
        QPainterPath path;
        path.moveTo(sourcePoint.x(), sourcePoint.y());
        QPointF ep(destPoint.x(), destPoint.y());
        path.cubicTo(bezier,bezier,ep);
        painter->drawPath(path);
        // Draw the arrows
        QLineF bLine(bezier,destPoint);
        //angle = std::atan2(-line.dy(), line.dx());
        angle = std::atan2(-bLine.dy(), bLine.dx());
      //  angle = std::atan2(-path.dy(), line.dx());

        /*
        destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI / 1.8) * qMin(arrowSize, line.length()));
        destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()),
                                                  cos(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, line.length()));

                                                  */

        destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 1.8) * qMin(arrowSize, bLine.length()),
                                                  cos(angle - M_PI / 1.8) * qMin(arrowSize, bLine.length()));
        destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, bLine.length()),
                                                  cos(angle - M_PI + M_PI / 1.8) * qMin(arrowSize, bLine.length()));
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
    painter->drawText(textPoint, textEdge);
}
