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
#include <QList>
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
    // GUI
    QPushButton *btnCreateNode, *btnConnectNode, *btnDelete;
    QLabel *lNameGraf,  // Для вывода имя графа: Мили или Мура
            *lTip;      // Подсказки для всего
    GraphWidget *grafViewScene;
    QLabel *lTipInput;   // Подсказки ввода
    QLineEdit *eInput;
    QPushButton *btnApply;   // Применить ввод (с проверкой корректности).
    QPushButton *btnCheck;   // Проверка всего графа с вариантом.
    // var and func
    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    bool connFlag;  // Флаг поднимается при нажатии на кнопку "Соединить"
    QList <Node*> nodes;
    QList <Edge*> edges;
    void designItem(QGraphicsItem *it);
signals:

public slots:
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void sceneSelectionChanged();
    void sceneSave();
};

#endif // WIN_H
