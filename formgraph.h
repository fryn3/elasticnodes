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
#include "automata.h"
#include "graphwidget.h"
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
    DlgInput *dlgInput;

    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    // Флаг поднимается при нажатии на кнопку "Соединить"
    // connFlag == 1 Надо отметить "Источник" потом "Получатель"
    // connFlag == 2 Надо отметить только "Получатель"
    int connFlag;
public:
    QList <Node*> nodes;
    QList <Edge*> edges;
    static FormGraph *openGraph(QString fileName, bool jsonFormat = false);
signals:

public slots:
    bool CreateAutomat(QStringList source);
    void CreateAutomat(Automata::Universal *_automat);
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
    void checkedTable();
    void checkedMatrixStr();
    bool saveGraph(QString fileName, bool jsonFormat = false) const;
};

#endif // FORMGRAPH_H
