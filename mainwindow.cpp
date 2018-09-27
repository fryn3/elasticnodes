#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    formGraph = ui->formGraph;
    connect(ui->actSave, &QAction::triggered, this, &MainWindow::graphSave);
    connect(ui->actSaveAs, &QAction::triggered, this, &MainWindow::graphSaveAs);
    connect(ui->actOpen, &QAction::triggered, this, &MainWindow::graphOpen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::graphSave() const {
    ui->formGraph->saveGraph("graph.graph");
}

void MainWindow::graphSaveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохрани граф",
                        "graph.graph",
                        "Graph Files (*.graph)");
    ui->formGraph->saveGraph(fileName);
}

void MainWindow::graphOpen() {
    QString fileName = QFileDialog::getOpenFileName(this,
                        "Выберете граф", "", "Graph Files (*.graph)");
    FormGraph *f = FormGraph::openGraph(fileName);
    if (!f) {
        qWarning("!f");
        return;
    }
    delete ui->formGraph;
    ui->formGraph = f;
    ui->gridLayout->addWidget(f, 0, 0, 1, 1);
    repaint();
}
