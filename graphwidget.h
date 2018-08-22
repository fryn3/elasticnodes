#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include "edge.h"
#include "node.h"

#include <math.h>

#include <QKeyEvent>
#include <QRandomGenerator>

class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);

    void itemMoved();
    void startBezier(Edge *e);
public slots:
    void addNode();
    void shuffle();
    void zoomIn();
    void zoomOut();


protected:
    void keyPressEvent(QKeyEvent *event) override;
    //void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;

#endif
    Edge *bezierEdge;
    void scaleView(qreal scaleFactor);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void editItem();


};

#endif // GRAPHWIDGET_H
