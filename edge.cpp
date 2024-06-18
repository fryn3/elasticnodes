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
int EdgeParent::_idStatic = 0;

EdgeParent::EdgeParent(Node *sourceNode, Node *destNode, QString text)
    : NodeEdgeParent(sourceNode->graph, _idStatic++, text),
      source(sourceNode), dest(destNode),
      arrowSize(15)
{
    if (!dest) {
        dest = source;
    }
    source->addEdge(this);
    if (source != dest) {
        dest->addEdge(this);
    }
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

EdgeParent::EdgeParent(GraphWidget *graphWidget)
    : NodeEdgeParent(graphWidget, 0),
      arrowSize(15)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
}

EdgeParent::~EdgeParent()
{
    source->removeEdge(this);
    if (source != dest)
        dest->removeEdge(this);
}

void EdgeParent::setTextContent(QString text)
{
    NodeEdgeParent::setTextContent(text);
    adjust();
}

void EdgeParent::adjust()
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

#define SIZE_POINT      (5)
QPainterPath EdgeParent::pathPoint(QPointF point) const {
    QPainterPath path;
    path.moveTo(point + QPointF(-SIZE_POINT, -SIZE_POINT));
    path.lineTo(point + QPointF(SIZE_POINT, -SIZE_POINT));
    path.lineTo(point + QPointF(SIZE_POINT, SIZE_POINT));
    path.lineTo(point + QPointF(-SIZE_POINT, SIZE_POINT));
    path.lineTo(point + QPointF(-SIZE_POINT, -SIZE_POINT));
    return path;
}

void EdgeParent::writeToJson(QJsonObject &json) const
{
    NodeEdgeParent::writeToJson(json);
    QJsonObject jsonIt = json["Item"].toObject();
    jsonIt["sourceId"] = source->id();
    jsonIt["destId"] = dest->id();
    json["Item"] = jsonIt;
}

void EdgeParent::readFromJson(const QJsonObject &json) {
    NodeEdgeParent::readFromJson(json);
    if (_idStatic <= _id) {
        _idStatic = _id + 1;
    }
    QJsonObject jsonIt = json["Item"].toObject();
    if (missKeys(jsonIt, QStringList { "sourceId", "destId"})) {
        return;
    }
    int sourceId = jsonIt["sourceId"].toInt();
    int destId = jsonIt["destId"].toInt();
    uint fl = 0; // флаг нахождение двух вершин
    for(auto it: graph->scene()->items()) {
        Node *n = dynamic_cast<Node*>(it);
        if (n) {
            if (n->id() == sourceId) {
                source = n;
                source->addEdge(this);
                fl |= 1;
            }
            if (n->id() == destId) {
                dest = n;
                dest->addEdge(this);  // проверка о повторении в addEdge
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
}

EdgeParent *EdgeParent::create(const QJsonObject &json, GraphWidget *graphWidget)
{
    QJsonObject jsonIt = json["Item"].toObject();
    if (jsonIt["Type"].toInt() == Edge::Type) {
        return new Edge(json, graphWidget);
    } else if (jsonIt["Type"].toInt() == EdgeCircle::Type) {
        return new EdgeCircle(json, graphWidget);
    }
    return nullptr;
}

QPolygonF EdgeParent::arrowPolygon(QPointF peak, qreal angle) const
{
    QPainterPath p;
    QLineF l1, l2;
    l1.setP1(peak);             l2.setP1(peak);
    l1.setLength(arrowSize);    l2.setLength(arrowSize);
    l1.setAngle(angle + 180 - 10);    l2.setAngle(angle + 180 + 10);
    return QPolygonF() << peak << l1.p2() << l2.p2();
}

QPainterPath EdgeParent::pathText() const {
    QPainterPath path;
    QPointF textPoint = posText();
    qreal x = textPoint.x();
    qreal y = textPoint.y();
    path.moveTo(x, y);
    path.lineTo(x, y - 18); // for QFont("Times", 11)
    path.lineTo(x + 8 * _textContent.size(), y - 18);
    path.lineTo(x + 8 * _textContent.size(), y + 4);
    path.lineTo(x, y + 4);
    path.lineTo(x, y);
    return path;
}

Edge::Edge(Node *sourceNode, Node *destNode, QString text)
    : EdgeParent(sourceNode, destNode, text)
{
    beforeLine.setPoints(source->pos(), dest->pos());   // от Источника до Получателя
    QLineF line1(source->pos(), dest->pos());
    line1.setLength(line1.length() / 2);
    QLineF line2(line1.p2(), line1.p1());
    QLineF line3 = line2.normalVector();
    // смещение при повторном соединении
    int countCopy = 0;
    foreach (auto e, source->edges()) {
        if (e->destNode() == dest) {
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
    : EdgeParent(graphWidget)
{
    readFromJson(json);
    adjust();
    graph->scene()->addItem(this);    // сразу добавляет на сцену
}

void Edge::writeToJson(QJsonObject &json) const
{
    EdgeParent::writeToJson(json);
    QJsonObject jsonIt = json["Item"].toObject();
    QJsonObject jsonBezier
    {
        {"x", bezier.x()},
        {"y", bezier.y()}
    };
    jsonIt["Bezier"] = jsonBezier;
    QJsonObject jsonLine
    {
            {"p1x", beforeLine.p1().x()},
            {"p1y", beforeLine.p1().y()},
            {"p2x", beforeLine.p2().x()},
            {"p2y", beforeLine.p2().y()},
    };
    jsonIt["line"] = jsonLine;
    json["Item"] = jsonIt;
}

void Edge::readFromJson(const QJsonObject &json)
{
    EdgeParent::readFromJson(json);
    QJsonObject jsonIt = json["Item"].toObject();
    if (missKeys(jsonIt, QStringList { "Bezier", "line" })) {
        return;
    }
    QJsonObject jsonBezier = jsonIt["Bezier"].toObject();
    if (missKeys(jsonBezier, QStringList { "x", "y" })) {
        return;
    }
    double x = jsonBezier["x"].toDouble();
    double y = jsonBezier["y"].toDouble();
    bezier.setX(x);
    bezier.setY(y);
    QJsonObject jsonLine = jsonIt["line"].toObject();
    if (missKeys(jsonLine,
                QStringList { "p1x", "p1y", "p2x", "p2y" })) {
        return;
    }
    beforeLine.setLine(jsonLine["p1x"].toDouble(), jsonLine["p1y"].toDouble(),
                       jsonLine["p2x"].toDouble(), jsonLine["p2y"].toDouble());
}

QPointF Edge::posText() const
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
        widthText = 8 * _textContent.size();
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

QPointF Edge::posTextWeight() const
{
    QPointF textPoint;
    if (source != dest) {
        QPointF midpoint = (source->pos() + dest->pos()) / 2;
        qreal angle = qAtan2(dest->pos().y() - source->pos().y(), dest->pos().x() - source->pos().x());
        qreal offset = Node::Radius + 10; 
        qreal offsetX = offset * qCos(angle);
        qreal offsetY = offset * qSin(angle);
        QPointF adjustedMidpoint = midpoint + QPointF(offsetX, offsetY);

        textPoint = adjustedMidpoint;
    } else { // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
    }
    return textPoint;
}

QPointF Edge::posTextName() const
{
    QPointF textPoint;
    if (source != dest) {
        QPointF midpoint = (source->pos() + dest->pos()) / 2;
        qreal angle = qAtan2(dest->pos().y() - source->pos().y(), dest->pos().x() - source->pos().x());
        qreal offset = Node::Radius + 10; 
        qreal offsetX = offset * qCos(angle);
        qreal offsetY = offset * qSin(angle);
        QPointF adjustedMidpoint = midpoint + QPointF(offsetX, offsetY);
        qreal oppositeAngle = angle + M_PI; 
        qreal oppositeOffsetX = offset * qCos(oppositeAngle);
        qreal oppositeOffsetY = offset * qSin(oppositeAngle);
        textPoint = adjustedMidpoint + QPointF(oppositeOffsetX, oppositeOffsetY);
    } else { // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
    }
    return textPoint;
}

QPainterPath Edge::pathBezierCurve() const {
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
    return path;
}

// Для столкновений и выделения
QPainterPath Edge::shape() const {
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    if (source != dest) {
        if (!isSelected()) {
            path = pathBezierCurve().united(pathText());
        } else {
            path = pathPoint(newPosBezier());
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
        QPointF textPoint = posText();
        qreal x = textPoint.x();
        qreal y = textPoint.y();
        pText << QPointF(x, y)
              << QPointF(x, y - 18) // for QFont("Times", 11)
              << QPointF(x + 8 * _textContent.size(), y - 18)
              << QPointF(x + 8 * _textContent.size(), y + 4)
              << QPointF(x, y + 4);
        QPolygonF pBezier;
        pBezier << bezier + QPointF(-1, -1)
                << bezier + QPointF(2, 0)
                << bezier + QPointF(0, 2)
                << bezier + QPointF(-2, 0);
        return pathBezierCurve().boundingRect()
                .united(pText.boundingRect())
                .united(pathPoint(newPosBezier()).boundingRect());
    }
    return shape().boundingRect();
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!source || !dest)
        return;
    double angle;
    QPointF peak, destArrowP1, destArrowP2;
    painter->setPen(QPen((isSelected() ? Qt::cyan: Qt::black), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF bezierPaint = newPosBezier();
    QLineF line(bezierPaint, mapFromScene(dest->pos()));
    if (source != dest) {
        QPainterPath myPath;
        myPath.moveTo(mapFromScene(source->pos()));
        QPointF bezierPaint = newPosBezier();
        myPath.cubicTo(bezierPaint, bezierPaint, mapFromScene(dest->pos()));

        painter->drawPath(myPath);
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
     // Draw the edge name
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10)); // Adjust the font and size as needed
    painter->drawText(posTextName(), getName());

   // Draw the weight near the edge
    QPointF weightPos = posTextWeight(); // Calculate the position for the weight

   // Set the font and color for the weight text
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setPen(Qt::black);
    //Draw the weight text at the calculated position
    painter->drawText(weightPos, QString::number(EdgeParent::weight));

    painter->setBrush((isSelected() ? Qt::cyan: Qt::black));
//    painter->drawPolygon(QPolygonF() << peak << destArrowP1 << destArrowP2);
    painter->setPen(QPen((isSelected() ? Qt::cyan: Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPolygon(arrowPolygon(peak, line.angle()));
    painter->setFont(QFont("Times", 11));
    painter->drawText(posText(), _textContent);
    if (isSelected()) {
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
    default:
        break;
    };
    return QGraphicsItem::itemChange(change, value);
}

EdgeCircle::EdgeCircle(Node *sourceNode, QString text)
    : EdgeParent (sourceNode, sourceNode, text)
    , radiusCircle(Node::Radius)
{
    graph->scene()->addItem(this);    // сразу добавляет на сцену
}

EdgeCircle::EdgeCircle(const QJsonObject &json, GraphWidget *graphWidget)
    : EdgeParent(graphWidget)
{
    readFromJson(json);
    adjust();
    graph->scene()->addItem(this);    // сразу добавляет на сцену
}

void EdgeCircle::writeToJson(QJsonObject &json) const
{
    EdgeParent::writeToJson(json);
    QJsonObject jsonIt = json["Item"].toObject();
    jsonIt["radiusCircle"] = radiusCircle;
    QJsonObject jsonPos
    {
        {"x", pos().x()},
        {"y", pos().y()}
    };
    jsonIt["pos"] = jsonPos;
    json["Item"] = jsonIt;
}

void EdgeCircle::readFromJson(const QJsonObject &json)
{
    EdgeParent::readFromJson(json);
    QJsonObject jsonIt = json["Item"].toObject();
    if (missKeys(jsonIt, QStringList { "radiusCircle", "pos" })) {
        return;
    }
    radiusCircle = jsonIt["radiusCircle"].toDouble();
    QJsonObject jsonPos = jsonIt["pos"].toObject();
    if (missKeys(jsonPos, QStringList { "x", "y" })) {
        return;
    }
    double x = jsonPos["x"].toDouble();
    double y = jsonPos["y"].toDouble();
    setPos(x, y);
}

QPointF EdgeCircle::peakArrow() const
{
    QLineF lCC = lineCC();
    QPointF P0(lCC.p1()), P1(lCC.p2());
    qreal d = lCC.length();
    qreal a = (Node::Radius * Node::Radius - radiusCircle * radiusCircle + d * d) / (2 * d);
    qreal h = sqrt(Node::Radius * Node::Radius - a * a);
    QPointF P2   (P0.x() + a * (P1.x() - P0.x()) / d,
                    P0.y() + a * (P1.y() - P0.y()) / d);
    QPointF P3_1 (P2.x() - h * (P1.y() - P0.y()) / d,
                    P2.y() + h * (P1.x() - P0.x()) / d);
    return P3_1;
}

// Для столкновений и выделения
QPainterPath EdgeCircle::shape() const {
    QPainterPath path1;
    path1.addEllipse(mapFromScene(centerPos()),
                        radiusCircle + 2, radiusCircle + 2);
    QPainterPath path2;
    path2.addEllipse(mapFromScene(centerPos()),
                        radiusCircle - 2, radiusCircle - 2);
    QPainterPath path3 = path1.subtracted(path2);
    return path3.united(pathText());
}

// для отрисовки
QRectF EdgeCircle::boundingRect() const
{
    QRectF rCircle = enoughBoundingRect(shape().boundingRect());
    QRectF rArrow = enoughBoundingRect(arrowPolygon(peakArrow(), angleArrow()).boundingRect());
    QRectF rPoint = enoughBoundingRect(pathPoint(handlePos()).boundingRect());
    return rCircle.united(rArrow).united(rPoint);
}

void EdgeCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen((isSelected() ? Qt::cyan: Qt::black), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawEllipse(static_cast<int>(mapFromScene(centerPos()).x() - radiusCircle),
                     static_cast<int>(mapFromScene(centerPos()).y() - radiusCircle),
                     2 * int(radiusCircle),
                     2 * int(radiusCircle));
    painter->setBrush((isSelected() ? Qt::cyan: Qt::black));
    if (isSelected()) {
        painter->drawEllipse(handlePos(), SIZE_POINT - 1, SIZE_POINT - 1);  // размер точки
    }

       // Draw the edge name
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10)); // Adjust the font and size as needed
    painter->drawText(posTextName(), getName());
    // Draw the weight near the edge
    QPointF weightPos = posTextWeight(); // Calculate the position for the weight
    // Set the font and color for the weight text
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setPen(Qt::black);
    //Draw the weight text at the calculated position
    painter->drawText(weightPos, QString::number(EdgeParent::weight));
    painter->setPen(QPen((isSelected() ? Qt::cyan: Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPolygon(arrowPolygon(peakArrow(), angleArrow()));
    painter->setFont(QFont("Times", 11));
    painter->drawText(posText(), _textContent);
    NodeEdgeParent::paint(painter, option, widget);
}

QPointF EdgeCircle::handlePos() const
{
    QLineF lineToHandle;
    lineToHandle.setP1(mapFromScene(centerPos()));
    lineToHandle.setLength(radiusCircle);
    lineToHandle.setAngle(lineCC().angle());
    return lineToHandle.p2();
}

bool EdgeCircle::handlePress(QPointF posPress)
{
    QPainterPath p = pathPoint(handlePos());
    return p.contains(posPress);
}

void EdgeCircle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    flHandlePress = handlePress(event->pos());
    EdgeParent::mousePressEvent(event);
}

void EdgeCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (flHandlePress) {
        qreal previousRad = radiusCircle;
        radiusCircle = QLineF(mapFromScene(centerPos()), event->pos()).length();
        if (!correctMove()) {
            radiusCircle = previousRad;
        }
        update();
    } else {
        EdgeParent::mouseMoveEvent(event);
    }
}

QPointF EdgeCircle::posText() const
{
    QLineF l1(mapFromScene(centerPos()), peakArrow());
    l1.setLength(l1.length() * 0.8);
    QLineF l2(mapFromScene(source->pos()), l1.p2());
    qreal anglRad = l2.angle() * PI / 180;
    qreal widthText, hightText;
    widthText = 8 * _textContent.size();
    hightText = 14;
    if (0 <= l2.angle() && l2.angle() < 180) {
        l2.setAngle(l2.angle() + 5);
    }

    if ((0 <= l2.angle() && l2.angle() < 90)) {
        l2.setLength(Node::Radius + 3);
    } else if (90 <= l2.angle() && l2.angle() < 180) {
        l2.setLength(Node::Radius + 3 + widthText * (-cos(anglRad)));
    } else if (180 <= l2.angle() && l2.angle() < 270) {
        l2.setLength(Node::Radius + 3 + widthText *(-cos(anglRad)) + hightText * (-sin(anglRad)));
    } else if (270 <= l2.angle() && l2.angle() < 360) {
        l2.setLength(Node::Radius + 3 + hightText * (-sin(anglRad)));
    }
    return l2.p2();
}
QPointF EdgeCircle::posTextWeight() const
{
    QPointF textPoint;
    if (source != dest) {
        QPointF midpoint = (source->pos() + dest->pos()) / 2;
        qreal angle = qAtan2(dest->pos().y() - source->pos().y(), dest->pos().x() - source->pos().x());
        qreal offset = Node::Radius + 10; 
        qreal offsetX = offset * qCos(angle);
        qreal offsetY = offset * qSin(angle);
        QPointF adjustedMidpoint = midpoint + QPointF(offsetX, offsetY);

        textPoint = adjustedMidpoint;
    } else { // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 2, boundingRect().center().y());
    }
    return textPoint;
}

QPointF EdgeCircle::posTextName() const
{
    QPointF textPoint;
    if (source != dest) {
        QPointF midpoint = (source->pos() + dest->pos()) / 2;
        qreal angle = qAtan2(dest->pos().y() - source->pos().y(), dest->pos().x() - source->pos().x());
        qreal offset = Node::Radius + 10; 
        qreal offsetX = offset * qCos(angle);
        qreal offsetY = offset * qSin(angle);
        QPointF adjustedMidpoint = midpoint + QPointF(offsetX, offsetY);
        qreal oppositeAngle = angle + M_PI; 
        qreal oppositeOffsetX = offset * qCos(oppositeAngle);
        qreal oppositeOffsetY = offset * qSin(oppositeAngle);
        textPoint = adjustedMidpoint + QPointF(oppositeOffsetX, oppositeOffsetY);
    } else { // source == dest
        textPoint = QPointF(boundingRect().center().x() - Node::Radius / 10, boundingRect().center().y());
    }
    return textPoint;
}
QVariant EdgeCircle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange:
    {
        qDebug() << ItemPositionChange;
        QLineF lpp(pos(), value.toPointF());
        qDebug() << lpp;
        while (!correctMove(lpp.p2())) {
            if (lpp.length() > 1) {
                lpp.setLength(lpp.length() - 1);
            } else {
                lpp.setP2(lpp.p1());
                break;
            }
        }
        return QGraphicsItem::itemChange(change, lpp.p2());
    }
    default:
        break;
    };
    return QGraphicsItem::itemChange(change, value);
}
