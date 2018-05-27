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
    if (data[TYPE_AUTO].toInt() == Type::MILI) {
        qDebug() << "I'm Mili";
        return new Mili(data);
    } else if (data[TYPE_AUTO].toInt() == Type::MURA) {
        qDebug() << "I'm Mura";
        return new Mura(data);
    } else {
        qDebug() << "I'm Abstract";
        return new Abstract(data);
    }
}


Abstract::Abstract(QStringList data) : countX(0), countY(0)
{
    if (data.size() < 4) {
        qDebug() << "Table did not get";
        type = Type::FAIL_TYPE;
        return;
    }
    outFile = data[NAME_FILE];
    // Мили или Мура
    type = data[TYPE_AUTO].toInt();
    countA = data[COLUMNS].toInt();
    int rows = data[ROWS].toInt();
    if (data.size() != rows * countA + COUNT_HEADER) {
        qDebug() << "Format table error";
        type = Type::FAIL_TYPE;
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

bool Abstract::check() { return false; }

Mili::Mili(QStringList data) : Abstract(data)
{
    if (type >= Type::FAIL_TYPE) {
        return;
    }
}

QRegExp Mura::regExpNode = QRegExp("a[0-9]+/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))");
QRegExp Mura::regExpEdge = QRegExp("x[1-9]+[0-9]*(,x[1-9]+[0-9]*)*");

Mura::Mura(QStringList data) : Abstract(data)
{
    if (type >= Type::FAIL_TYPE) {
        return;
    }
    countX = table.size() - 1;
    checkTable.resize(countA);
    for (int i = 0; i < countA; i++) {
        if (table[0][i] == "-") {
            checkTable[i].insert("y", 0);
        } else {
            QStringList yN = table[0][i].split(",");
            for(int j = 0; j < yN.size(); j++) {
                // "yN", где N это номер вых сигнала.
                int N = yN[j].remove(0, 1).toInt();
                if (countY < N) {
                    countY = N;
                }
                // "y1, y2" -> [1, 2]
                checkTable[i].insert("OutSignals", N);
            }
        }
        // Начиная со второй строки.
        for (int j = 1; j < countX + 1; j++) {
            if (table[j][i] == "-") {
                checkTable[i].insert("OutState", 0);
            } else {
                // "a3" -> 3
                int outSt = table[j][i].mid(1).toInt();
                checkTable[i].insert("OutState", outSt);
            }
        }
    }
}

}
