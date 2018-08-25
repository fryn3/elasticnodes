#include "automata.h"
namespace Automata {

Universal::Universal(QStringList data) : t(nullptr), f(nullptr) {
    int type, format;
    if (data.size() < FormatFile::COUNT_HEADER) {
        qDebug() << "Universal() : error data format";
        return;
    }
    type = data.at(FormatFile::TYPE).toInt();
    format = data.at(FormatFile::FORMAT).toInt();
    if (type == Mili::Type) {
        if (format == Table::Format) {
            auto v = new MiliTable(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(v);
        } else if (format == Matrix::Format) {
            auto v = new MiliMatrix(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(v);
        }
    } else if (type == Mura::Type) {
        if (format == Table::Format) {
            auto v = new MuraTable(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(v);
        } else if (format == Matrix::Format) {
            auto v = new MuraMatrix(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(v);
        }
    }
    if (!t || !f)
        qDebug() << "Universal() : in dynamic_cast is error";
}

bool Universal::check(QVector<QMultiMap<QString, int> > ch)
{
    if (t->type() == Mili::Type) {
        MiliTable *obj = dynamic_cast<MiliTable*>(t);
        if (!obj) {
            qDebug() << "check() : dynamic_cast fail";
            return false;
        }
        return  obj->check(ch);
    } else if (t->type() == Mura::Type) {
        MuraTable *obj = dynamic_cast<MuraTable*>(t);
        if (!obj) {
            qDebug() << "check() : dynamic_cast fail";
            return false;
        }
        return obj->check(ch);
    }
    qDebug() << "check() : unknown type";
    return false;
}

bool Universal::check(QStringList ch)
{
    Universal u(ch);
    return u == *this;
}

bool operator==(const Universal &u1, const Universal &u2)
{
    return *u1.f == *u2.f;
}

bool operator==(const Format &f1, const Format &f2)
{
    return f1.data == f2.data;
}

Format::Format(QStringList source)
    : countA(0), countX(0), countY(0), _fail(false)
{
    if (source.size() < FormatFile::COUNT_HEADER) {
        qDebug() << "Table : source.size() < COUNT_HEADER";
        _fail = true;
        return;
    }
    countA = source.at(FormatFile::COLUMNS).toInt();
    int rows = source[FormatFile::ROWS].toInt();
    if (source.size() != rows * countA + FormatFile::COUNT_HEADER) {
        qDebug() << "Table : source.size() != rows * countA + COUNT_HEADER";
        _fail = true;
        return;
    }
    data.resize(rows);
    for (int i = 0; i < rows; i++) {
        data[i].resize(countA);
        for(int j = 0; j < countA; j++) {
            data[i][j] = source[i * countA + j + FormatFile::COUNT_HEADER];
        }
    }
}

Table::Table(QStringList source) : Automata::Format (source) { }

Matrix::Matrix(QStringList source) : Automata::Format (source) { }

MiliTable::MiliTable(QStringList source) : Table(source)
{
    if (_fail)
        return;
    countX = data.size();
    _check.resize(countA);
    for (int i = 0; i < countA; i++) {
        for (int j = 1; j < countX + 1; j++) {
            // "-/-", "a3/y1,y3", "a2/-"
            QStringList numStr = data[j - 1][i].split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            if (numStr.size() == 0) {
                _check[i].insert(QString("x%1a").arg(j), 0);
                _check[i].insert(QString("x%1y").arg(j), 0);
            } else if (numStr.size() == 1) {
                _check[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
                _check[i].insert(QString("x%1y").arg(j), 0);
            } else {
                _check[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
                for (int k = 1; k < numStr.size(); k++) {
                    _check[i].insert(QString("x%1y").arg(j), numStr.at(k).toInt());
                    if (countY < numStr.at(k).toInt()) {
                        countY = numStr.at(k).toInt();
                    }
                }
            }
        }
    }
}

MuraTable::MuraTable(QStringList source) : Table(source)
{
    if (_fail)
        return;
    countX = data.size() - 1;
    countA = data.at(0).size();
    _check.resize(countA);
    for (int i = 0; i < countA; i++) {
        if (data[0][i] == "-") {
            _check[i].insert("y", 0);
        } else {
            QStringList yN = data[0][i].split(",");
            for(int j = 0; j < yN.size(); j++) {
                // "yN", где N это номер вых сигнала.
                int N = yN[j].mid(1).toInt();
                if (countY < N) {
                    countY = N;
                }
                // "y1, y2" -> [1, 2]
                _check[i].insert("y", N);
            }
        }
        // Начиная со второй строки.
        for (int j = 1; j < countX + 1; j++) {
            // "x1" : [0]
            if (data[j][i] == "-") {
                _check[i].insert(QString("x%1").arg(j), 0);
            } else {
                // "a3" -> 3
                int outSt = data[j][i].mid(1).toInt();
                _check[i].insert(QString("x%1").arg(j), outSt);
            }
        }
    }
}

MiliMatrix::MiliMatrix(QStringList source) : Matrix (source)
{
    if (_fail)
        return;
    for (auto i = 0; i < countA; i++) {
        QVector<QPair<int, QList<int> > > line;
        for (auto j = 0; j < countA; j++) {
            QStringList numStr = data.at(i).at(j).split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            QList<int> y;
            if (numStr.size() == 0) {
                y.append(0);
                line.append(qMakePair(0, y));
            } else {
                int xTemp = numStr.at(0).toInt();
                if (xTemp > countX) {
                    countX = xTemp;
                }
                if (numStr.size() == 1) {
                    y.append(0);
                    line.append(qMakePair(xTemp, y));
                } else {
                    for (auto k = 1; k < numStr.size(); k++) {
                        int yTemp = numStr.at(k).toInt();
                        y.append(yTemp);
                        if (yTemp > countY) {
                            countY = yTemp;
                        }
                    }
                    line.append(qMakePair(xTemp, y));
                }
            }
        }
        matrix.append(line);
    }
}

MuraMatrix::MuraMatrix(QStringList source) : Matrix (source)
{
    if (_fail)
        return;
    for (auto i = 0; i < data.at(0).size(); i++) {
        QList<int> t;
        if (data[0][i] == "-") {
            Y.append(t);
        } else {
            QStringList yN = data[0][i].split(",");
            for(auto j = 0; j < yN.size(); j++) {
                // "yN", где N это номер вых сигнала.
                int N = yN[j].mid(1).toInt();
                if (countY < N) {
                    countY = N;
                }
                t.append(N);
            }
            qSort(t.begin(), t.end());
            Y.append(t);
        }
    }
    for (auto i = 1; i < data.size(); i++) {
        QVector<int> line;
        for (auto j = 0; j < data.at(0).size(); j++) {
            if (data.at(i).at(j) == "-") {
                line.append(0);
            } else {
                // "x3 -> 3
                int x = data.at(i).at(j).mid(1).toInt();
                line.append(x);
                if (x > countX) {
                    countX = x;
                }
            }
        }
        matrixC.append(line);
    }
}

}
