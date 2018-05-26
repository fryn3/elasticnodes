#ifndef AUTOMATA_H
#define AUTOMATA_H
#include <QDebug>
#include <QVector>
#include <QList>
#include <QMultiMap>
namespace Automata {
namespace Type {
enum {
    MILI,
    MURA,
    FAIL_TYPE
};
}
class Abstract;
class Mili;
class Mura;
Abstract *create(QStringList data);

class Abstract
{
public:
    Abstract(QStringList data);
    QString outFile;
    QVector<QVector<QString> > table;   // Таблица графа
    int countA;                         // Кол-во состояний
    int countX;                         // Кол-во вых. сигн
    int countY;                         // Кол-во вход. сигн
    int type;   // TypeAutomat
    // Проверяет корректность графа.
    // В классе Abstract всегда возвращает false
    virtual bool check();
};

class Mili : public Abstract
{
public:
    Mili(QStringList data);
};

class Mura : public Abstract
{
public:
    Mura(QStringList data);
    QVector<QMultiMap<QString, int> > checkTable;
};

}

#endif // AUTOMATA_H
