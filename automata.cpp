#include "automata.h"
namespace Automata {
enum HeaderData {
    FULL_NAME_EXE,
    NAME_FILE,  // номер варианта
    TYPE_AUTO,
    COLUMNS,     // кол-во состояний
    ROWS,
    COUNT_HEADER
};

Abstract *create(QStringList data)
{
    if (data.size() < COUNT_HEADER) {
        qDebug() << "Запуск без нужных параметров!";
        return new Abstract(data);
    }else if (data[TYPE_AUTO].toInt() == Mili::Type) {
        qDebug() << "I'm Mili";
        return new Mili(data);
    } else if (data[TYPE_AUTO].toInt() == Mura::Type) {
        qDebug() << "I'm Mura";
        return new Mura(data);
    } else {
        qDebug() << "I'm Abstract";
        return new Abstract(data);
    }
}


Abstract::Abstract(QStringList data) : countX(0), countY(0), _fail(false)
{
    if (data.size() < 4) {
        qDebug() << "Table did not get";
        _fail = true;
        return;
    }
    outFile = data[NAME_FILE];
    countA = data[COLUMNS].toInt();
    int rows = data[ROWS].toInt();
    if (data.size() != rows * countA + COUNT_HEADER) {
        qDebug() << "Format table error";
        _fail = true;
        return;
    }
    table.resize(rows);
    for (int i = 0; i < rows; i++) {
        table[i].resize(countA);
        for(int j = 0; j < countA; j++) {
            table[i][j] = data[i * countA + j + COUNT_HEADER];
        }
    }
}

bool Abstract::check(QVector<QMultiMap<QString, int> > checkTable) const { return (_checkTable == checkTable); }

bool Abstract::fail() const { return _fail; }

QRegExp Abstract::regExpNode() const { return QRegExp(""); }

QRegExp Abstract::regExpEdge() const { return QRegExp(""); }

QString Abstract::tipNode() const { return ""; }

QString Abstract::tipEdge() const { return ""; }

const QRegExp Mili::forNode = QRegExp("a[0-9]+");
const QRegExp Mili::forEdge = QRegExp("x[1-9]+[0-9]*/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))");

Mili::Mili(QStringList data) : Abstract(data)
{
    if (_fail)
        return;
    countX = table.size();
    _checkTable.resize(countA);
    for (int i = 0; i < countA; i++) {
        for (int j = 1; j < countX + 1; j++) {
            // "-/-", "a3/y1,y3", "a2/-"
            QStringList numStr = table[j - 1][i].split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            if (numStr.size() == 0) {
                _checkTable[i].insert(QString("x%1a").arg(j), 0);
                _checkTable[i].insert(QString("x%1y").arg(j), 0);
            } else if (numStr.size() == 1) {
                _checkTable[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
                _checkTable[i].insert(QString("x%1y").arg(j), 0);
            } else {
                _checkTable[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
                for (int k = 1; k < numStr.size(); k++) {
                    _checkTable[i].insert(QString("x%1y").arg(j), numStr.at(k).toInt());
                    if (countY < numStr.at(k).toInt()) {
                        countY = numStr.at(k).toInt();
                    }
                }
            }
        }
    }
}

QRegExp Mili::regExpNode() const { return forNode; }

QRegExp Mili::regExpEdge() const { return forEdge; }

QString Mili::tipNode() const
{
    return QString("Шаблон ввода: aK, где K от 0 до %1")
            .arg(countA - 1);
}

QString Mili::tipEdge() const
{
    return QString("Шаблон ввода: xK/yL[,yM] или xK/-, где K от 0 до %1, L и M от 1 до %2")
            .arg(countX).arg(countY);
}

const QRegExp Mura::forNode = QRegExp("a[0-9]+/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))");
const QRegExp Mura::forEdge = QRegExp("x[1-9]+[0-9]*(,x[1-9]+[0-9]*)*");

Mura::Mura(QStringList data) : Abstract(data)
{
    if (_fail)
        return;
    countX = table.size() - 1;
    _checkTable.resize(countA);
    for (int i = 0; i < countA; i++) {
        if (table[0][i] == "-") {
            _checkTable[i].insert("y", 0);
        } else {
            QStringList yN = table[0][i].split(",");
            for(int j = 0; j < yN.size(); j++) {
                // "yN", где N это номер вых сигнала.
                int N = yN[j].remove(0, 1).toInt();
                if (countY < N) {
                    countY = N;
                }
                // "y1, y2" -> [1, 2]
                _checkTable[i].insert("y", N);
            }
        }
        // Начиная со второй строки.
        for (int j = 1; j < countX + 1; j++) {
            // "x1" : [0]
            if (table[j][i] == "-") {
                _checkTable[i].insert(QString("x%1").arg(j), 0);
            } else {
                // "a3" -> 3
                int outSt = table[j][i].mid(1).toInt();
                _checkTable[i].insert(QString("x%1").arg(j), outSt);
            }
        }
    }
}

QRegExp Mura::regExpNode() const { return forNode; }

QRegExp Mura::regExpEdge() const { return forEdge; }

QString Mura::tipNode() const
{
    return QString("Шаблон ввода: aK/yL[,yM] или aK/-, где K от 0 до %1, L и M от 1 до %2")
            .arg(countA - 1).arg(countY);
}

QString Mura::tipEdge() const
{
    return QString("Шаблон ввода: aK, где K от 0 до %1")
            .arg(countA - 1);
}

}
