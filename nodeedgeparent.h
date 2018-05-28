#ifndef NODEEDGEPARENT_H
#define NODEEDGEPARENT_H
#include <QString>
#include <QGraphicsItem>
// абстрактный класс
class NodeEdgeParent : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };
    virtual void setTextContent(QString text) = 0;
    virtual QString textContent() const = 0;
};

#endif // NODEEDGEPARENT_H
