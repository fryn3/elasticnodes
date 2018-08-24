#include "automata.h"
namespace Automata {
enum HeaderDataFTable {
//    FULL_NAME_EXE,
//    NAME_FILE,  // номер варианта
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

Universal *create(const QStringList data, int formatInputData)
{
    int indexType;
    if (formatInputData == Table::Format) {
        indexType = TYPE_AUTO;
    } else if (formatInputData == Matrix::Format) {
        indexType = 0;
    } else {
        qDebug() << "create : formatInputData is not correct";
        return nullptr;
    }
    if (indexType >= data.size()) {
        qDebug() << "create : data is not correct";
        return nullptr;
    }
    return new Universal(data, data.at(indexType).toInt(), formatInputData);
}

Format::Format(const QStringList source)
    : countA(0), countX(0), countY(0), _fail(false)
{
    if (source.size() < COUNT_HEADER) {
        qDebug() << "Table : source.size() < COUNT_HEADER";
        _fail = true;
        return;
    }
    countA = source[COLUMNS].toInt();
    int rows = source[ROWS].toInt();
    if (source.size() != rows * countA + COUNT_HEADER) {
        qDebug() << "Table : source.size() != rows * countA + COUNT_HEADER";
        _fail = true;
        return;
    }
    data.resize(rows);
    for (int i = 0; i < rows; i++) {
        data[i].resize(countA);
        for(int j = 0; j < countA; j++) {
            data[i][j] = source[i * countA + j + COUNT_HEADER];
        }
    }
}

Table::Table(const QStringList source) : Automata::Format (source) { }

Matrix::Matrix(const QStringList source) : Automata::Format (source) { }

MiliTable::MiliTable(const QStringList source) : Table(source)
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

MuraTable::MuraTable(const QStringList source) : Table(source)
{
    if (_fail)
        return;
    countX = data.size() - 1;
    _check.resize(countA);
    for (int i = 0; i < countA; i++) {
        if (data[0][i] == "-") {
            _check[i].insert("y", 0);
        } else {
            QStringList yN = data[0][i].split(",");
            for(int j = 0; j < yN.size(); j++) {
                // "yN", где N это номер вых сигнала.
                int N = yN[j].remove(0, 1).toInt();
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

MiliMatrix::MiliMatrix(const QStringList data) : Matrix (data)
{

}

MuraMatrix::MuraMatrix(const QStringList data) : Matrix (data)
{

}








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
