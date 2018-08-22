#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QList>

#include "win.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    //Окно приложения
    Win *widget = new Win;

    //ВАЖНО!!!!
    QStringList * automatdata = new QStringList();
    automatdata->push_back("proga.exe");
    automatdata->push_back("Automat");

    //Список игриков
    QStringList *ylist = new QStringList();
    //Список иксов
    QStringList *xlist = new QStringList();

    //Если graph.txt есть,то забьем его
    QFile in(QCoreApplication::applicationDirPath()+"/graph.txt");
     if (in.open(QIODevice::ReadOnly | QIODevice::Text))
     {
          QTextStream file(&in);
         //Тип автомата
         int auto_type = file.readLine().toInt();
         automatdata->push_back(QString::number(auto_type)); // Кладём тип автомата к ВАЖНОМУ!!
         //Размер ашек и тд
         int auto_size = file.readLine().toInt();
         automatdata->push_back(QString::number(auto_size)); // Кладём количество переходных состояний (ашек) к ВАЖНОМУ!!
         automatdata->push_back(QString::number(auto_size)); // Кладём ЕЩЕ РАЗ о_О
         if (auto_type == 1) {
             //Автомат Мура..
             //widget->lNameGraf->setText("Автомат Мура");
             //Считаем массив игриков
             for (int i=0;i<auto_size;i++)
             {
                QString tmp = file.readLine();
                ylist->push_back(tmp);
                //ЗАПИСЫВАЕМ В ВАЖНОЕ!!! СПЕРВА ИГРИКИ

                    //А не, не записываем из-за automata::конструктора
                    //automatdata->push_back(tmp);
                //widget->onBtnCreateNodeClicked();
                //widget->nodes.at(i)->setTextContent("a"+QString::number(i)+"/"+tmp);
             }
            //Массив иксов:
             for (int i=0;i<auto_size;i++)
                 for (int j=0;j<auto_size;j++)
                 {
                     QString tmp = file.readLine();
                     xlist->push_back(tmp);
                     //ЗАПИСЫВАЕМ В ВАЖНОЕ!!! ЗАТЕМЫ ИКСЫ
                     automatdata->push_back(tmp);
                 }
             //Ребра массива иксов
             for (int i=0;i<auto_size;i++)
             {
                 for (int j=0;j<auto_size;j++)
                 {
                     if (xlist->at(i*auto_size+j) != "-")
                     {
                         //qDebug()<<"Связь есть:"<<xlist->at(i*auto_size+j)<<"Между: "<<i<<" и "<<j<<endl;
                         //Edge *e = new Edge(widget->nodes.at(i), widget->nodes.at(j),xlist->at(i*auto_size+j));
                         //widget->edges.append(e);
                         //widget->grafViewScene->scene()->addItem(e);
                     }
                 }
             }
         }
         else
         {
             //Автомат Мили..
             //qDebug()<<"Автомат мили, размер ашек:"<<auto_size<<endl;
             //widget->lNameGraf->setText("Автомат Мили");
             //Только массив иксов
             for (int i=0;i<auto_size;i++)
                 for (int j=0;j<auto_size;j++)
                 {
                     QString tmp = file.readLine();
                     xlist->push_back(tmp);
                     //ЗАПИСЫВАЕМ В ВАЖНОЕ!!! ИКСЫЫ
                     automatdata->push_back(tmp);
                 }
             //Массив ашек заполним
             //Считаем массив игриков
             for (int i=0;i<auto_size;i++)
             {
                //widget->onBtnCreateNodeClicked();
                //widget->nodes.at(i)->setTextContent("a"+QString::number(i));
             }
             //Ребра массива иксов
             for (int i=0;i<auto_size;i++)
             {
                 for (int j=0;j<auto_size;j++)
                 {
                     if (xlist->at(i*auto_size+j) != "-/-")
                     {
                         qDebug()<<"Связь есть:"<<xlist->at(i*auto_size+j)<<"Между: "<<i<<" и "<<j<<endl;
                         //Edge *e = new Edge(widget->nodes.at(i), widget->nodes.at(j),xlist->at(i*auto_size+j));
                         //widget->edges.append(e);
                         //widget->grafViewScene->scene()->addItem(e);
                     }
                 }
             }
         }
         qDebug()<<"Размер:"<<auto_size<<"Тип:"<<auto_type<<endl;
     }
     else
     {
         qDebug()<<"Запуск без файла graph.txt. Директория,откуда запускается программа: "<<QCoreApplication::applicationDirPath() <<endl;
     }

    widget->CreateAutomat(*automatdata);
    qDebug()<<"AutomatData"<<*automatdata<<endl;
    widget->show();
    widget->setWindowTitle("GE");
    return app.exec();
}
