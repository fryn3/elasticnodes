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
#include "dlginput.h"

namespace Ui {
class FormGraph;
}

class FormGraph : public QWidget
{
    Q_OBJECT

public:
    explicit FormGraph(QWidget *parent = nullptr);
    ~FormGraph() override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::FormGraph *ui;
    DlgInput *dlgInput;

    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    // Флаг поднимается при нажатии на кнопку "Соединить"
    // connFlag == 1 Надо отметить "Источник" потом "Получатель"
    // connFlag == 2 Надо отметить только "Получатель"
    int connFlag;

    QVector<QVector<int>> adjacencyMatrix;
    QVector<QVector<int>> incidenceMatrix;
    
public:
    QList <Node*> nodes;
    QList <EdgeParent*> edges;
    static FormGraph *openGraph(QString fileName, bool jsonFormat = false);
signals:

public slots:
    void showInput();
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void eInputTextChange();
    void onBtnApplyClicked();
    void sceneSelectionChanged();
    void savePng(QString fileName) const;
    bool saveGraph(QString fileName, bool jsonFormat = false) const;
    void keyPressEvent(QKeyEvent *event) override;

    QString getInputText(const QString &message);
    void initMatrix();
    void updateMatrixDisplay();
    void initMatrixIncidence();
    void updateMatrixDisplayIncidence();
private slots:
    void on_displayMatrix_clicked();

    void on_incidenceMatrix_clicked();
};

#endif // FORMGRAPH_H
