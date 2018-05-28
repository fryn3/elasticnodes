#ifndef AUTOMATA_H
#define AUTOMATA_H
#include <QDebug>
#include <QVector>
#include <QList>
#include <QMultiMap>
namespace Automata {

class Abstract;
class Mili;
class Mura;
Abstract *create(QStringList data);

class Abstract
{
public:
    Abstract(QStringList data);
    virtual ~Abstract() = default;
    enum { Type = -1 };
    virtual int type() const { return Type; }
    QString outFile;
    // Таблица графа в строковом представлении
    QVector<QVector<QString> > table;
    int countA;                         // Кол-во состояний
    int countX;                         // Кол-во вых. сигн. В абстр классе == 0!
    int countY;                         // Кол-во вход. сигн. В абстр классе == 0!
    // Проверяет корректность графа.
    bool check(QVector<QMultiMap<QString, int> > checkTable) const;
    bool fail() const;
    virtual QRegExp regExpNode() const;
    virtual QRegExp regExpEdge() const;
    virtual QString tipNode() const;
    virtual QString tipEdge() const;
protected:
    // Таблица графа в численном представлении
    QVector<QMultiMap<QString, int> > _checkTable;
    bool _fail;
};

class Mili : public Abstract
{
public:
    Mili(QStringList data);
    enum { Type = 0 };
    int type() const override { return Type; }
    QRegExp regExpNode() const override;
    QRegExp regExpEdge() const override;
    QString tipNode() const override;
    QString tipEdge() const override;
private:
    // Проверяет корректность ввода с помощью QString::contains
    static const QRegExp forNode;
    static const QRegExp forEdge;
};

class Mura : public Abstract
{
public:
    Mura(QStringList data);
    enum { Type = 1 };
    int type() const override { return Type; }
    QRegExp regExpNode() const override;
    QRegExp regExpEdge() const override;
    QString tipNode() const override;
    QString tipEdge() const override;
private:
    // Проверяет корректность ввода с помощью QString::contains
    static const QRegExp forNode;
    static const QRegExp forEdge;
};

}

#endif // AUTOMATA_H
