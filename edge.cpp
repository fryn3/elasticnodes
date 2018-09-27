#include "edge.h"
#include "node.h"
#include "graphwidget.h"
#include <QString>
#include <qmath.h>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

const qreal PI = atan(1) * 4;
int Edge::_idStatic = 0;

Edge::Edge(Node *sourceNode, Node *destNode, QString textArrow)
    : NodeEdgeParent(sourceNode->graph),
      _id(_idStatic++), textEdge(textArrow),
      arrowSize(15), flSelected(false)
{
    setFlag(ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    if (source != dest) {
        dest->addEdge(this);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
    }
    beforeLine.setPoints(source->pos(), dest->pos());   // от Источника до Получателя
    QLineF line1(source->pos(), dest->pos());
    line1.setLength(line1.length() / 2);
    QLineF line2(line1.p2(), line1.p1());
    QLineF line3 = line2.normalVector();
    // смещение при повторном соединении
    int countCopy = 0;
    foreach (auto e, source->edges()) {
        if (e->dest == dest) {
            countCopy++;
        }
    }
    line3.setLength(pow(-1, countCopy) * countCopy * 20);
    bezier.setX(line3.p2().x());
    bezier.setY(line3.p2().y());                        // bezier
    adjust();
    graph->scene()->addItem(this);    // сразу добавляет на сцену
}

Edge::Edge(const QJsonObject &json, GraphWidget *graphWidget)
    : NodeEdgeParent(graphWidget),
      arrowSize(15), flSelected(false)
{
    readFromJson(json);
    setFlag(ItemIsSelectable);
    source->addEdge(this);
    if (source != dest) {
        dest->addEdge(this);
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
    }
    adjust();
    graph->scene()->addItem(this);    // сразу добавляет на сцену
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

void Edge::writeToJson(QJsonObject &json) const
{
    QJsonObject jsonEdge;
    jsonEdge["id"] = _id;
    jsonEdge["textContent"] = textContent();
    jsonEdge["sourceId"] = source->id();
    jsonEdge["destId"] = dest->id();
    QJsonObject jsonBezier
    {
        {"x", bezier.x()},
        {"y", bezier.y()}
    };
    jsonEdge["Bezier"] = jsonBezier;
    QJsonObject jsonLine
    {
            {"p1x", beforeLine.p1().x()},
            {"p1y", beforeLine.p1().y()},
            {"p2x", beforeLine.p2().x()},
            {"p2y", beforeLine.p2().y()},
    };
    jsonEdge["line"] = jsonLine;

    json["Edge"] = jsonEdge;
}

void Edge::readFromJson(const QJsonObject &json)
{
    if (missKey(json, "Edge")) {
        return;
    }
    QJsonObject jsonEdge = json["Edge"].toObject();
    if (missKey(jsonEdge, "id") || missKey(jsonEdge, "textContent")
            || missKey(jsonEdge, "sourceId") || missKey(jsonEdge, "destId")
            || missKey(jsonEdge, "Bezier") || missKey(jsonEdge, "line")) {
        return;
    }
    _id = int(jsonEdge["id"].toDouble());
    if (_idStatic <= _id) {
        _idStatic = _id + 1;
    }
    textEdge = jsonEdge["textContent"].toString();  // не вызывать setContent()
    int sourceId = jsonEdge["sourceId"].toInt();
    int destId = jsonEdge["destId"].toInt();
    uint fl = 0; // флаг нахождение двух вершин
    for(auto it: graph->scene()->items()) {
        Node *n = dynamic_cast<Node*>(it);
        if (n) {
            if (n->id() == sourceId) {
                source = n;
                fl |= 1;
            }
            if (n->id() == destId) {
                dest = n;
                fl |= 2;
            }
            if (fl == 3) {  // 2 флага подняты
                break;
            }
        }
    }
    if (fl != 3) {
        qDebug() << "fl != 3";
    }
    QJsonObject jsonBezier = jsonEdge["Bezier"].toObject();
    if (missKey(jsonBezier, "x") || missKey(jsonBezier, "y")) {
        return;
    }
    double x = jsonBezier["x"].toDouble();
    double y = jsonBezier["y"].toDouble();
    bezier.setX(x);
    bezier.setY(y);
    QJsonObject jsonLine = jsonEdge["line"].toObject();
    if (missKey(jsonLine, "p1x") || missKey(jsonLine, "p1y")
            || missKey(jsonLine, "p2x") || missKey(jsonLine, "p2y")) {
        return;
    }
    beforeLine.setLine(jsonLine["p1x"].toDouble(), jsonLine["p1y"].toDouble(),
                       jsonLine["p2x"].toDouble(), jsonLine["p2y"].toDouble());
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    prepareGeometryChange();
    if(source != dest) {
        for (auto edg : source->edges()) {
            if (edg->sourceNode() == dest) {
                if(_id > edg->_id) {
                    edg->adjust();
                }
                break;
            }
        }
    }
}

QPointF Edge::newPosText() const
{
    QPointF textPoint;
    if(source != dest) {
        // Нахождение точки для текста
        // for QFont("Times", 11)
        QPointF bezierNew = newPosBezier();
        QLineF line1(bezierNew, mapFromScene(dest->pos())), line2;
        line1.setLength(line1.length() - Node::Radius);
        qreal anglRad = line1.angle() * PI / 180;
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
            QLineF line11(line1.p2(), bezierNew);
            line11.setLength(5);
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(hightText * -cos(anglRad) + 5 * -sin(anglRad));
            textPoint = line3.p2();
        } else {
            // 5 / 5
            // widthText / 5
            QLineF line11(line1.p2(), bezierNew);
            line11.setLength(5 * -sin(anglRad) + widthText * cos(anglRad));
            line2.setPoints(line11.p2(), line11.p1());
            QLineF line3 = line2.normalVector();
            line3.setLength(5.0);
            textPoint = line3.p2();
        }
    } else {        // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
    }
    return textPoint;
}

int Edge::id() const
{
    return _id;
}

QPainterPath Edge::pathBezierCurve() const {    // + text
    QPainterPath path;
    qreal qOffset = 5;
    QPointF newBezier = newPosBezier();
    QLineF line0(mapFromScene(source->pos()), newBezier);
    QLineF line1(newBezier, mapFromScene(dest->pos()));
    qreal dx0 = qOffset * sin(line0.angle() * M_PI / 180);
    qreal dy0 = qOffset * cos(line0.angle() * M_PI / 180);
    qreal dx1 = qOffset * sin(line1.angle() * M_PI / 180);
    qreal dy1 = qOffset * cos(line1.angle() * M_PI / 180);
    QPointF offset0(dx0, dy0);
    QPointF offset1(dx1, dy1);
    path.moveTo(mapFromScene(source->pos() + offset0));
    path.cubicTo(newBezier + (offset0 + offset1) / 2,
                 newBezier + (offset0 + offset1) / 2,
                 mapFromScene(dest->pos()) + offset1);
    path.lineTo(mapFromScene(dest->pos()) - offset1);
    path.cubicTo(newBezier - (offset0 + offset1) / 2,
                 newBezier - (offset0 + offset1) / 2,
                 mapFromScene(source->pos()) - offset0);
    path.lineTo(mapFromScene(source->pos()) + offset0);
    // Text
    QPointF textPoint = newPosText();
    qreal x = textPoint.x();
    qreal y = textPoint.y();
    path.moveTo(x, y);
    path.lineTo(x, y - 18); // for QFont("Times", 11)
    path.lineTo(x + 8 * textEdge.size(), y - 18);
    path.lineTo(x + 8 * textEdge.size(), y + 4);
    path.lineTo(x, y + 4);
    path.lineTo(x, y);
    return path;
}

#define SIZE_POINT      (5)
QPainterPath Edge::pathBezierPoint() const {
    QPainterPath path;
    QPointF newBezier = newPosBezier();
    path.moveTo(newBezier + QPointF(-SIZE_POINT, -SIZE_POINT));
    path.lineTo(newBezier + QPointF(SIZE_POINT, -SIZE_POINT));
    path.lineTo(newBezier + QPointF(SIZE_POINT, SIZE_POINT));
    path.lineTo(newBezier + QPointF(-SIZE_POINT, SIZE_POINT));
    path.lineTo(newBezier + QPointF(-SIZE_POINT, -SIZE_POINT));
    return path;
}

// Для столкновений и выделения
QPainterPath Edge::shape() const {
    QPainterPath path;
    if (source != dest) {
        if (!flSelected) {
            path = pathBezierCurve();
        } else {
            path = pathBezierPoint();
        }
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
        QPointF textPoint = newPosText();
        qreal x = textPoint.x();
        qreal y = textPoint.y();
        pText << QPointF(x, y)
              << QPointF(x, y - 18) // for QFont("Times", 11)
              << QPointF(x + 8 * textEdge.size(), y - 18)
              << QPointF(x + 8 * textEdge.size(), y + 4)
              << QPointF(x, y + 4);
        QPolygonF pBezier;
        pBezier << bezier + QPointF(-1, -1)
                << bezier + QPointF(2, 0)
                << bezier + QPointF(0, 2)
                << bezier + QPointF(-2, 0);
        return pathBezierCurve().boundingRect()
                .united(pText.boundingRect())
                .united(pathBezierPoint().boundingRect());
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
    QPointF bezierPaint = newPosBezier();
    if (source != dest) {
        QPainterPath myPath;
        myPath.moveTo(mapFromScene(source->pos()));
        QPointF bezierPaint = newPosBezier();
        myPath.cubicTo(bezierPaint, bezierPaint, mapFromScene(dest->pos()));

        painter->drawPath(myPath);
        QLineF line(bezierPaint, mapFromScene(dest->pos()));
        line.setLength(line.length() - Node::Radius);
        if (qFuzzyCompare(line.length(), qreal(0.)))
            return;

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
                         16 * 180, 16 * -270);
        angle = 1.07 * M_PI;
        destArrowP1 = dest->pos()+ QPointF(sin(angle - M_PI / 1.8) * arrowSize + Node::Radius,
                                                 cos(angle - M_PI / 1.8) * arrowSize);
        destArrowP2 = dest->pos() + QPointF(sin(angle - M_PI + M_PI / 1.8) * arrowSize + Node::Radius,
                                                 cos(angle - M_PI + M_PI / 1.8) * arrowSize);
        painter->setBrush((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black));
        peak = dest->pos() + QPointF(Node::Radius, 0);

    }
    painter->setBrush((option->state & QStyle::State_Selected ? Qt::cyan: Qt::black));
    painter->drawPolygon(QPolygonF() << peak << destArrowP1 << destArrowP2);
    painter->setFont(QFont("Times", 11));
    painter->drawText(newPosText(), textEdge);
    if (flSelected) {
        painter->drawEllipse(bezierPaint, SIZE_POINT - 1, SIZE_POINT - 1);  // размер точки
    }
    NodeEdgeParent::paint(painter, option, widget);
}

QPointF Edge::newPosBezier() const
{
        QLineF line1(mapFromScene(source->pos()), mapFromScene(dest->pos()));
        QLineF beforeBezier(beforeLine.p1(), bezier);
        QLineF line2 = line1;
        line2.setAngle(line1.angle() - (beforeLine.angle() - beforeBezier.angle()));
        line2.setLength(line1.length() / beforeLine.length() * beforeBezier.length());
        return line2.p2();
}

QVariant Edge::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange:
        bezier = newPosBezier();
        break;
    case ItemPositionHasChanged:
        beforeLine.setPoints(mapFromScene(source->pos()), mapFromScene(dest->pos()));
        adjust();
        break;
    case ItemSelectedChange:
        flSelected = value.toBool();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
