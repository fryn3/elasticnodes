#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QList>
#include <QDebug>
#include "formgraph.h"

const QString MILI = "/MiliMatrix.txt";
const QString MURA = "/MuraMatrix.txt";
const QString FOR_PROGRAMM = "/graph.txt";
const QString SELECTED_FILE = FOR_PROGRAMM;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    FormGraph *widget = new FormGraph;

    QFile in(QCoreApplication::applicationDirPath() + SELECTED_FILE);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << QString("Запуск без файла %1.").arg(SELECTED_FILE);
        qDebug() << "Директория,откуда запускается программа: "
               << QCoreApplication::applicationDirPath() << endl;
        return 101;
    }
    QTextStream stream(&in);
    QStringList source;
    while (true) {
        QString line = stream.readLine();
        if (line.isEmpty()) {
            break;
        }
        source.append(line);
    }

    // Заглушка! Всегда type = matrix
    source.insert(Automata::FormatFile::FORMAT, "1");

    // Вставляем кол-во строк. У Мура на одну строку больше.
    if (source.at(Automata::FormatFile::TYPE).toInt() == Automata::Mili::Type) {
        source.insert(Automata::FormatFile::ROWS, QString("%1")
                      .arg(source.at(Automata::FormatFile::COLUMNS).toInt()));
    } else if (source.at(Automata::FormatFile::TYPE).toInt() == Automata::Mura::Type) {
        source.insert(Automata::FormatFile::ROWS, QString("%1")
                      .arg(source.at(Automata::FormatFile::COLUMNS).toInt() + 1));
    } else {
        qDebug() << "main : Automata::FormatFile::TYPE error";
        return 102;
    }
    if (!widget->CreateAutomat(source)) {
        qDebug() << "main : !widget->CreateAutomat(source))";
        return 103;
    }
    widget->setWindowTitle("GE");
    widget->show();
    return app.exec();
}
