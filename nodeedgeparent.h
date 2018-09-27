#ifndef NODEEDGEPARENT_H
#define NODEEDGEPARENT_H
#include <QString>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
//#define DEBUG_NODE_EDGE

inline bool missKey(const QJsonObject &json, QString key) {
    if (!json.contains(key)) {
        qWarning() << "QJsonObject не содержит " << key;
        return true;
    }
    return false;
}

class GraphWidget;

// абстрактный класс
class NodeEdgeParent : public QGraphicsItem
{
public:
    NodeEdgeParent(GraphWidget *graphWidget);
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    virtual int id() const = 0;
    virtual void setTextContent(QString text) = 0;
    virtual QString textContent() const = 0;
    virtual void writeToJson(QJsonObject &json) const = 0;
    virtual void readFromJson(const QJsonObject &json) = 0;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    GraphWidget *graph;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};


#endif // NODEEDGEPARENT_H

