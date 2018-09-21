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
    GraphWidget(QWidget *parent = nullptr);

    void itemMoved();
public slots:
    void addNode();
    void shuffle();
    void zoomIn();
    void zoomOut();
    void writeToJson(QJsonObject &json) const;
    void readFromJson(const QJsonObject &json);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    //void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void scaleView(qreal scaleFactor);
    void mouseReleaseEvent(QMouseEvent *event) override;
signals:
    void editItem();


};

#endif // GRAPHWIDGET_H
