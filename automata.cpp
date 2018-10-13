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

Universal *Universal::readFromJson(const QJsonObject &json)
{
    if (missKey(json, "Automat") || !json["Automat"].isObject()) {
        return nullptr;
    }
    QJsonObject jsonA = json["Automat"].toObject();
    if (missKey(jsonA, "source") || !jsonA["source"].isArray()) {
        return nullptr;
    }
    QStringList source;
    QJsonArray jsonArray = jsonA["source"].toArray();
    foreach (auto obj, jsonArray) {
        source.append(obj.toString());
    }
    return new Universal(source);
}

void Universal::writeToJson(QJsonObject &json) const
{
    f->writeToJson(json);
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
    return f1.dataInt == f2.dataInt;
}

void Format::writeToJson(QJsonObject &json) const
{
    QJsonObject jsonFormat;
    QJsonArray jsonArray;
    foreach (auto str, _source) {
        jsonArray.append(QJsonValue(str));
    }
    jsonFormat["source"] = jsonArray;
    json["Automat"] = jsonFormat;
}

Format::Format(QStringList source)
    : countA(0), countX(0), countY(0), _source(source), _fail(false)
{   // countX, countY устанавлиавть в потомках
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
    dataStr.resize(rows);
    for (int i = 0; i < rows; i++) {
        dataStr[i].resize(countA);
        for(int j = 0; j < countA; j++) {
            dataStr[i][j] = source[i * countA + j + FormatFile::COUNT_HEADER];
        }
    }

    foreach (auto lineStr, dataStr) {
        QVector<QList<int> > lineInt;
        foreach (auto str, lineStr) {
            auto numStr = listNums(str);
            QList<int>  cell;
            foreach (auto n, numStr) {
                cell.append(n.toInt());
            }
            // Сортировку делать в классах потомках
            lineInt.append(cell);
        }
        dataInt.append(lineInt);
    }
}

Table::Table(QStringList source) : Automata::Format (source) { }

Matrix::Matrix(QStringList source) : Automata::Format (source) { }

MiliTable::MiliTable(QStringList source) : Table(source)
{
    if (_fail)
        return;
    countX = dataInt.size();
    // Сортировка, countY
    for (auto row = 0; row < dataInt.size(); row++) {
        for (auto col = 0; col < dataInt.at(0).size(); col++) {
            // Первое число не сортировать
            qSort(dataInt[row][col].begin() + 1, dataInt[row][col].end());
            for (auto pY = dataInt[row][col].begin() + 1; pY != dataInt[row][col].end(); pY++) {
                if (*pY > countY) {
                    countY = *pY;
                }
            }
        }
    }
    _check.resize(countA);
    for (int i = 0; i < countA; i++) {
        for (int j = 1; j < countX + 1; j++) {
            // "-/-", "a3/y1,y3", "a2/-"
            QStringList numStr = listNums(dataStr[j - 1][i]);
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
    countX = dataInt.size() - 1;
    // Сортировка первой строки, countY
    for (auto col = 0; col < dataInt.at(0).size(); col++) {
        qSort(dataInt[0][col].begin(), dataInt[0][col].end());
        foreach (auto y, dataInt.at(0).at(col)) {
            if (y > countY) {
                countY = y;
            }
        }
    }

    _check.resize(countA);
    for (int i = 0; i < countA; i++) {
        if (dataStr[0][i] == "-") {
            _check[i].insert("y", 0);
        } else {
            QStringList yN = dataStr[0][i].split(",");
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
            if (dataStr[j][i] == "-") {
                _check[i].insert(QString("x%1").arg(j), 0);
            } else {
                // "a3" -> 3
                int outSt = dataStr[j][i].mid(1).toInt();
                _check[i].insert(QString("x%1").arg(j), outSt);
            }
        }
    }
}

MiliMatrix::MiliMatrix(QStringList source) : Matrix (source)
{
    if (_fail)
        return;
    // Сортировка
    for (auto row = 0; row < dataInt.size(); row++) {
        for (auto col = 0; col < dataInt.at(0).size(); col++) {
            // Первое число ПОСЛЕ ЗАПЯТОЙ не сортировать
            // x1/y1; -/-; x1/y2,x2/y1,y2
            QStringList edges = dataStr[row][col].split(",x", QString::SkipEmptyParts);
            std::sort(edges.begin(), edges.end(), [](QString s1, QString s2) {
                    return listNums(s1).at(0).toInt() < listNums(s2).at(0).toInt();});

            dataInt[row][col].clear();
            foreach(auto edge, edges) {
                QList<int> lInt;
                auto edgeNum = listNums(edge);
                foreach (auto n, edgeNum) {
                    lInt.append(n.toInt());
                }
                if (lInt.size() > 0) {
                    std::sort(lInt.begin() + 1, lInt.end());
                }
                dataInt[row][col].append(lInt);
                dataInt[row][col].append(-1);
            }
//            qSort(dataInt[row][col].begin() + 1, dataInt[row][col].end());    // так было. Надо удалить
        }
    }
    // countX, countY
    for (auto row = dataInt.begin(); row != dataInt.end(); row++) { // х2/-; -/-; -/-; -/-; х1/у4; -/-; х1/у1,у5;
        for (auto cell = row->begin(); cell != row->end(); cell++) {
            if (cell->size() == 1 * 2 * cell->count(-1)) {
                auto x = cell->at(0);
                if (x > countX) {
                    countX = x;
                }
            } else if (cell->size() > 1 * 2 * cell->count(-1)) {
                for (auto pY = cell->begin() + 1; pY != cell->end(); pY++) {
                    if (*pY != -1 && *pY > countY) {
                        countY = *pY;
                    }
                }
            }
        }
    }
}

MuraMatrix::MuraMatrix(QStringList source) : Matrix (source)
{
    if (_fail)
        return;
    // Сортировка
    for (auto row = 0; row < dataInt.size(); row++) {
        for (auto col = 0; col < dataInt.at(0).size(); col++) {
            qSort(dataInt[row][col].begin(), dataInt[row][col].end());
        }
    }
    // countY
    foreach (auto cellY, dataInt.at(0)) {   // y1,y5; y2; -; y3,y1;
        foreach (auto y, cellY) {
            if (y > countY) {
                countY = y;
            }
        }
    }
    // countX
    for (auto row = dataInt.begin() + 1; row != dataInt.end(); row++) { // x2; -; -; x3; -
        for (auto cellX = row->begin(); cellX != row->end(); cellX++) {
            foreach (auto x , *cellX) {
                if (x > countX) {
                    countX = x;
                }
            }
        }
    }
}

}
