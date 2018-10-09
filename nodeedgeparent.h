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
    NodeEdgeParent(GraphWidget *graphWidget, QString text = nullptr);
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    int id() const;
    virtual void setTextContent(QString text);
    virtual QString textContent() const { return _textContent; }
    virtual void writeToJson(QJsonObject &json) const;
    virtual void readFromJson(const QJsonObject &json);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    GraphWidget *graph;
protected:
    QString _textContent;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    int _id;
    // область перерисовки с запасом
    QRectF enoughBoundingRect(QRectF rect) const;
};


#endif // NODEEDGEPARENT_H

