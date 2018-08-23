#ifndef AUTOMATA_H
#define AUTOMATA_H
#include <QDebug>
#include <QVector>
#include <QList>
#include <QMultiMap>
namespace Automata {

class Type;
class Mili;
class Mura;
class Format;
class Table;
class Matrix;
class MiliTable;
class MiliMatrix;
class MuraTable;
class MuraMatrix;

// Тут можно попробовать использовать union
struct Universal {
    Type *t;
    Format *f;
    Universal(const QStringList data, int type, int format);
};

Universal *create(const QStringList data, int formatInputData);

class Type
{
public:
    virtual ~Type() = default;
    virtual int type() const = 0;
    virtual QRegExp regExpNode() const = 0;
    virtual QRegExp regExpEdge() const = 0;
    virtual QString tipNode() const = 0;
    virtual QString tipEdge() const = 0;
};

class Mili : public Type
{
public:
    enum { Type = 0 };
    int type() const override { return Type; }
    QRegExp regExpNode() const override { return QRegExp("a(0|[1-9][0-9]*)"); }
    QRegExp regExpEdge() const override { return QRegExp("x[1-9]+[0-9]*/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))"); }
    QString tipNode() const override { return "Шаблон ввода: aK"; }
    QString tipEdge() const override { return "Шаблон ввода: xK/yL[,yM] или xK/-"; }
};

class Mura : public Type
{
public:
    enum { Type = 1 };
    int type() const override { return Type; }
    QRegExp regExpNode() const override { return QRegExp("a(0|[1-9][0-9]*)/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))"); }
    QRegExp regExpEdge() const override { return QRegExp("x[1-9]+[0-9]*(,x[1-9]+[0-9]*)*"); }
    QString tipNode() const override { return "Шаблон ввода: aK/yL[,yM] или aK/-"; }
    QString tipEdge() const override { return "Шаблон ввода: aK"; }
};

class Format
{
public:
    virtual ~Format() = default;
    virtual int format() const = 0;
    bool fail() const { return _fail; }
protected:
    Format();
    bool _fail;
};

class Table : public Format
{
public:
    enum { Format = 0 };
    int format() const override { return Format; }
    int countA;                         // Кол-во состояний
    int countX;                         // Кол-во вых. сигн. В абстр классе == 0!
    int countY;                         // Кол-во вход. сигн. В абстр классе == 0!
    bool check(QVector<QMultiMap<QString, int> > checkTable) const { return _checkTable == checkTable; }
protected:
    QVector<QMultiMap<QString, int> > _checkTable;
};

class Matrix : public Format
{
public:
    enum { Format = 1 };
    int format() const override { return Format; }
    bool check(QVector<QVector<int> > checkTable) const { return _checkTable == checkTable; }
protected:
    QVector<QVector<int> > _checkTable;
};

class MiliTable : public Mili, public Table
{
public:
    MiliTable(QStringList data);
};

class MiliMatrix : public Mili, public Matrix
{
public:
    MiliMatrix(QStringList data);
};

class MuraTable : public Mura, public Table
{
public:
    MuraTable(QStringList data);
};

class MuraMatrix : public Mura, public Matrix
{
public:
    MuraMatrix(QStringList data);
};

}

#endif // AUTOMATA_H
