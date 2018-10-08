#include "graphwidget.h"

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 500, 500);
    setMinimumSize(500, 500);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
}

void GraphWidget::addNode()
{
    static int x = 0, y = -100;
    Node *node1 = new Node(this);
    scene()->addItem(node1);
    switch ((Node::idStatic() - 1) % 2) {
    case 0:
        x -= 2 * Node::Radius + 10;
        y += 2 * Node::Radius + 10;
        break;
    case 1:
        x += 2 * Node::Radius + 10;
        break;
    }
    node1->setPos(x, y);
}

void GraphWidget::mousePressEvent(QMouseEvent *event){
    switch(event->button()){
    case Qt::RightButton:
        if (scene()->selectedItems().size()>0){
            //qDebug()<<" Clicked right";
            emit GraphWidget::editItem();
        }
        break;

    default:

        break;
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    Node *selectedNode = nullptr;
    if (scene()->selectedItems().size() == 1) {
        selectedNode = qgraphicsitem_cast<Node *> (scene()->selectedItems().at(0));
    }
    switch (event->key()) {
    case Qt::Key_Up:
        if (selectedNode)
            selectedNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        if (selectedNode)
            selectedNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        if (selectedNode)
            selectedNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        if (selectedNode)
            selectedNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;

    case Qt::RightButton:
        //emit editItem();
        //QGraphicsView::keyPressEvent(event);
        break;
    default:

        QGraphicsView::keyPressEvent(event);
    }
}

#if QT_CONFIG(wheelevent)
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow(double(2), -event->delta() / 240.0));
}
#endif

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void GraphWidget::writeToJson(QJsonObject &json) const
{
    QJsonArray jsonArray;
    jsonArray.append(transform().m11());
    jsonArray.append(transform().m12());
    jsonArray.append(transform().m13());
    jsonArray.append(transform().m21());
    jsonArray.append(transform().m22());
    jsonArray.append(transform().m23());
    jsonArray.append(transform().m31());
    jsonArray.append(transform().m32());
    jsonArray.append(transform().m33());
    QJsonObject jsonWidget;
    jsonWidget["scale"] = jsonArray;
    json["GraphWidget"] = jsonWidget;
}

void GraphWidget::readFromJson(const QJsonObject &json)
{
    if (json.contains("GraphWidget") && json["GraphWidget"].isObject()) {
        QJsonObject jsonWid = json["GraphWidget"].toObject();
        if (jsonWid.contains("scale") && jsonWid["scale"].isArray()) {
            QJsonArray jsonArray = jsonWid["scale"].toArray();
            QTransform tr(jsonArray.at(0).toDouble(),
                          jsonArray.at(1).toDouble(),
                          jsonArray.at(2).toDouble(),
                          jsonArray.at(3).toDouble(),
                          jsonArray.at(4).toDouble(),
                          jsonArray.at(5).toDouble(),
                          jsonArray.at(6).toDouble(),
                          jsonArray.at(7).toDouble(),
                          jsonArray.at(8).toDouble());
            setTransform(tr);
        } else {
            qDebug() << "QJsonObject не содержит scale";
        }
    } else {
        qDebug() << "QJsonObject не содержит GraphWidget";
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event){
    QGraphicsView::mouseReleaseEvent(event);
}
