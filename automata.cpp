#include "automata.h"
namespace Automata {
enum HeaderDataFTable {
    FULL_NAME_EXE,
    NAME_FILE,  // номер варианта
    TYPE_AUTO,
    COLUMNS,     // кол-во состояний
    ROWS,
    COUNT_HEADER
};

Universal::Universal(const QStringList data, int type, int format) : t(nullptr), f(nullptr) {
    if (type == Mili::Type) {
        if (format == Table::Format) {
            auto v = new MiliTable(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(f);
        } else if (format == Matrix::Format) {
            auto v = new MiliMatrix(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(f);
        }
    } else if (type == Mura::Type) {
        if (format == Table::Format) {
            auto v = new MuraTable(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(f);
        } else if (format == Matrix::Format) {
            auto v = new MuraMatrix(data);
            t = dynamic_cast<Type*>(v);
            f = dynamic_cast<Format*>(f);
        }
    }

    if (!t || !f)
        qDebug() << "Universal() : in dynamic_cast is error";
}

Universal *create(const QStringList data, int formatInputData)
{
    Universal *uni;
    int indexType;
    if (formatInputData == Table::Format) {
        indexType = TYPE_AUTO;
    } else if (formatInputData == Matrix::Format) {
        indexType = 0;
    } else {
        qDebug() << "FormatError";
        return nullptr;
    }
    return new Universal(data, data[indexType].toInt(), formatInputData);
}

Format::Format() : _fail(false) { }






//AutoFTable::AutoFTable(QStringList data) :
//    Format(data, formatInputData), countX(0), countY(0)
//{
//    if (format == FormatTable) {
//        if (data.size() < 4) {
//            qDebug() << "Table did not get";
//            _fail = true;
//            return;
//        }
//        outFile = data[NAME_FILE];
//        countA = data[COLUMNS].toInt();
//        int rows = data[ROWS].toInt();
//        qDebug()<<data.size()<<endl<<rows<<endl<<countA<<COUNT_HEADER<<endl;
//        if (data.size() != rows * countA + COUNT_HEADER) {
//            qDebug() << "Format table error";
//            _fail = true;
//            return;
//        }
//        table.resize(rows);
//        for (int i = 0; i < rows; i++) {
//            table[i].resize(countA);
//            for(int j = 0; j < countA; j++) {
//                table[i][j] = data[i * countA + j + COUNT_HEADER];
//            }
//        }
//    } else if (format == FormatMatrix) {

//    } else {
//        qDebug() << "Format table error";
//        _fail = true;
//    }
//}

//bool AutoFTable::check(QVector<QMultiMap<QString, int> > checkTable) const { return (_checkTable == checkTable); }

//const QRegExp Mili::forNode = QRegExp("a(0|[1-9][0-9]*)");
//const QRegExp Mili::forEdge = QRegExp("x[1-9]+[0-9]*/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))");

//MiliFTable::MiliFTable(QStringList data, int formatInputData) :
//    AutoFTable(data, formatInputData)
//{
//    if (_fail)
//        return;
//    countX = table.size();
//    _checkTable.resize(countA);
//    for (int i = 0; i < countA; i++) {
//        for (int j = 1; j < countX + 1; j++) {
//            // "-/-", "a3/y1,y3", "a2/-"
//            QStringList numStr = table[j - 1][i].split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
//            if (numStr.size() == 0) {
//                _checkTable[i].insert(QString("x%1a").arg(j), 0);
//                _checkTable[i].insert(QString("x%1y").arg(j), 0);
//            } else if (numStr.size() == 1) {
//                _checkTable[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
//                _checkTable[i].insert(QString("x%1y").arg(j), 0);
//            } else {
//                _checkTable[i].insert(QString("x%1a").arg(j), numStr.at(0).toInt());
//                for (int k = 1; k < numStr.size(); k++) {
//                    _checkTable[i].insert(QString("x%1y").arg(j), numStr.at(k).toInt());
//                    if (countY < numStr.at(k).toInt()) {
//                        countY = numStr.at(k).toInt();
//                    }
//                }
//            }
//        }
//    }
//}

//QRegExp MiliFTable::regExpNode() const { return forNode; }

//QRegExp MiliFTable::regExpEdge() const { return forEdge; }

//QString MiliFTable::tipNode() const
//{
//    return QString("Шаблон ввода: aK, где K от 0 до %1")
//            .arg(countA - 1);
//}

//QString MiliFTable::tipEdge() const
//{
//    return QString("Шаблон ввода: xK/yL[,yM] или xK/-, где K от 0 до %1, L и M от 1 до %2")
//            .arg(countX).arg(countY);
//}

//const QRegExp MuraFTable::forNode = QRegExp("a(0|[1-9][0-9]*)/((y[1-9]+[0-9]*(,y[1-9]+[0-9]*)*)|(-))");
//const QRegExp MuraFTable::forEdge = QRegExp("x[1-9]+[0-9]*(,x[1-9]+[0-9]*)*");

//MuraFTable::MuraFTable(QStringList data, int formatInputData) : AutoFTable(data, formatInputData)
//{
//    if (_fail)
//        return;
//    countX = table.size() - 1;
//    _checkTable.resize(countA);
//    for (int i = 0; i < countA; i++) {
//        if (table[0][i] == "-") {
//            _checkTable[i].insert("y", 0);
//        } else {
//            QStringList yN = table[0][i].split(",");
//            for(int j = 0; j < yN.size(); j++) {
//                // "yN", где N это номер вых сигнала.
//                int N = yN[j].remove(0, 1).toInt();
//                if (countY < N) {
//                    countY = N;
//                }
//                // "y1, y2" -> [1, 2]
//                _checkTable[i].insert("y", N);
//            }
//        }
//        // Начиная со второй строки.
//        for (int j = 1; j < countX + 1; j++) {
//            // "x1" : [0]
//            if (table[j][i] == "-") {
//                _checkTable[i].insert(QString("x%1").arg(j), 0);
//            } else {
//                // "a3" -> 3
//                int outSt = table[j][i].mid(1).toInt();
//                _checkTable[i].insert(QString("x%1").arg(j), outSt);
//            }
//        }
//    }
//}

//QRegExp MuraFTable::regExpNode() const { return forNode; }

//QRegExp MuraFTable::regExpEdge() const { return forEdge; }

//QString MuraFTable::tipNode() const
//{
//    return QString("Шаблон ввода: aK/yL[,yM] или aK/-, где K от 0 до %1, L и M от 1 до %2")
//            .arg(countA - 1).arg(countY);
//}

//QString MuraFTable::tipEdge() const
//{
//    return QString("Шаблон ввода: aK, где K от 0 до %1")
//            .arg(countA - 1);
//}



}
