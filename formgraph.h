#ifndef FORMGRAPH_H
#define FORMGRAPH_H

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMultiMap>
#include <QSpacerItem>
#include <QTableWidget>
#include <QHeaderView>
#include <QList>
#include <QMessageBox>
#include "graphwidget.h"
#include "automata.h"
#include "dlginput.h"

namespace Ui {
class FormGraph;
}

class FormGraph : public QWidget
{
    Q_OBJECT

public:
    explicit FormGraph(QWidget *parent = nullptr);
    ~FormGraph();
    void closeEvent(QCloseEvent *event);
    Automata::Universal *automat;

private:
    Ui::FormGraph *ui;

//    QPushButton *btnCreateNode, *btnConnectNode, *btnDelete;

//    QPushButton *btnCheck;   // Проверка всего графа с вариантом.
    DlgInput *dlgInput;

    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    // Флаг поднимается при нажатии на кнопку "Соединить"
    // connFlag == 1 Надо отметить "Источник" потом "Получатель"
    // connFlag == 2 Надо отметить только "Получатель"
    int connFlag;
public:
    Edge * _edge;
    QList <Node*> nodes;

//    QLabel *lNameGraf,  // Для вывода имя графа: Мили или Мура
//    *lTip;      // Подсказки для всего
//    GraphWidget *grafViewScene;
    QList <Edge*> edges;
signals:

public slots:
    bool CreateAutomat(QStringList source);
    void showInput();
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void eInputTextChange();
    void onBtnApplyClicked();
    void onBtnCheckClicked();
    void sceneSelectionChanged();
    void sceneSave();
    void dropEvent(QDropEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //Загрузить автомат
    //    void loadAuto(int type,QStringList ylist,QStringList xlist,int size);
    void checkedTable();
    void checkedMatrixStr();
};

#endif // FORMGRAPH_H
