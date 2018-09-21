#ifndef AUTOMATA_H
#define AUTOMATA_H
#include <QDebug>
#include <QVector>
#include <QList>
#include <QMultiMap>
#include <QJsonObject>
#include <QJsonArray>


inline QStringList listNums(QString str) { return str.split(QRegExp("[^0-9]"), QString::SkipEmptyParts); }

namespace Automata {

namespace FormatFile {
enum {
    TYPE,       // mili/mura
    FORMAT,     // table/matrix
    COLUMNS,    // кол-во состояний
    ROWS,
    COUNT_HEADER
};
}

class Type;
class Mili;
class Mura;
class Format;
class Table;
class Matrix;
class MiliTable;
class MuraTable;
class MiliMatrix;
class MuraMatrix;

// Для указателей можно попробовать использовать union
struct Universal {
    Type *t;
    Format *f;
    Universal(const QStringList data);
    static Universal *readFromJson(const QJsonObject &json);
    bool check(QVector<QMultiMap<QString, int> > ch);
    bool check(QVector<QVector<int> > ch);
    // Пока работает только для Matrix
    bool check(QStringList ch);
    friend bool operator== (const Universal &u1, const Universal &u2);
};

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
    int countA;                         // Кол-во состояний
    int countX;                         // Кол-во вых. сигн. В абстр классе == 0!
    int countY;                         // Кол-во вход. сигн. В абстр классе == 0!
    friend bool operator== (const Format &f1, const Format &f2);
    void writeToJson(QJsonObject &json) const;
protected:
    Format(const QStringList source);
    QStringList _source;
    bool _fail;
    QVector<QVector<QString> > dataStr;
    QVector<QVector<QList<int> > > dataInt;
};

class Table : public Format
{
public:
    enum { Format = 0 };
    int format() const override { return Format; }
    bool check(QVector<QMultiMap<QString, int> > check) const { return _check == check; }
protected:
    Table(const QStringList source);
    QVector<QMultiMap<QString, int> > _check;
};

class Matrix : public Format
{
public:
    enum { Format = 1 };
    int format() const override { return Format; }
protected:
    Matrix(const QStringList source);
};

class MiliTable : public Mili, public Table
{
public:
    MiliTable(const QStringList source);
};

class MuraTable : public Mura, public Table
{
public:
    MuraTable(const QStringList source);
};

class MiliMatrix : public Mili, public Matrix
{
public:
    MiliMatrix(const QStringList source);
};

class MuraMatrix : public Mura, public Matrix
{
public:
    MuraMatrix(const QStringList source);
};

}

#endif // AUTOMATA_H
