#ifndef WIN_H
#define WIN_H

#include <QApplication>
#include <QWidget>
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
#include "graphwidget.h"
#include "automata.h"


class Win : public QWidget
{
    Q_OBJECT
public:
    explicit Win(QWidget *parent = nullptr);
    ~Win();
    Automata::Abstract *automat;
private:
    QPushButton *btnCreateNode, *btnConnectNode, *btnDelete;
    QLabel *lNameGraf,  // Для вывода имя графа: Мили или Мура
            *lTip;      // Подсказки
    GraphWidget *grafViewScene;
    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    bool connFlag;  // Флаг поднимается при нажатии на кнопку "Соединить"
signals:

public slots:
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void sceneSelectionChanged();
    void sceneSave();
};

#endif // WIN_H
