#ifndef NODEEDGEPARENT_H
#define NODEEDGEPARENT_H
#include <QString>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#define DEBUG

class GraphWidget;

// абстрактный класс
class NodeEdgeParent : public QGraphicsItem
{
public:
    NodeEdgeParent(GraphWidget *graphWidget);
    enum { Type = UserType + 1 };
    virtual void setTextContent(QString text) = 0;
    virtual QString textContent() const = 0;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    GraphWidget *graph;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};


#endif // NODEEDGEPARENT_H

