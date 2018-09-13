#include "formgraph.h"
#include "ui_formgraph.h"

FormGraph::FormGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGraph)
{
    ui->setupUi(this);
}

FormGraph::~FormGraph()
{
    delete ui;
}
