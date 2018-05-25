#ifndef WIN_H
#define WIN_H

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
#include <QApplication>

enum TypeAutomat {
    Mili,
    Mura
};

class Win : public QWidget
{
    Q_OBJECT
public:
    explicit Win(QWidget *parent = nullptr);
    ~Win();
    QVector<QVector<QString> > table;   // таблица графа
    int typeAuto;   // TypeAutomat
private:
    QPushButton *btnCreateNode, *btnConnectNode, *btnDelete;
    QLabel *lNameGraf;
    GraphWidget *grafViewScene;
    Node *_source;  // сохраняет вершину при нажатии на кнопку "Соединить"
    bool connFlag;  // флаг поднимается при нажатии на кнопку "Соединить"
signals:

public slots:
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void sceneSelectionChanged();
};

#endif // WIN_H
