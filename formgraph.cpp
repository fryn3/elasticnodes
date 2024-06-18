#include "formgraph.h"
#include "ui_formgraph.h"

#include <QJsonDocument>

FormGraph::FormGraph(QWidget *parent) :
    QWidget(parent), ui(new Ui::FormGraph),
    _source(nullptr), connFlag(false)
{
    ui->setupUi(this);

    dlgInput = new DlgInput();

    connect(ui->btnCreateNode, &QPushButton::clicked, this, &FormGraph::onBtnCreateNodeClicked);
    connect(ui->btnConnectNode, &QPushButton::clicked, this, &FormGraph::onBtnConnectNodeClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &FormGraph::onBtnDeleteClicked);
    connect(ui->grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &FormGraph::sceneSelectionChanged);
    connect(dlgInput->btnApply, &QPushButton::clicked, this, &FormGraph::onBtnApplyClicked);
    connect(ui->grafViewScene, &GraphWidget::editItem, this, &FormGraph::showInput);
}

FormGraph::~FormGraph()
{
    ui->grafViewScene->scene()->clearSelection();
    nodes.clear();
    edges.clear();
    delete ui;
    delete dlgInput;
}

void FormGraph::closeEvent(QCloseEvent */*event*/)
{
    dlgInput->close();
    // Важно! disconnect нужен для корректного выхода из приложения!
    disconnect(ui->grafViewScene->scene(), nullptr, nullptr, nullptr);
}

void FormGraph::showInput()
{
    if (ui->grafViewScene->scene()->selectedItems().size() == 0) {  // nullptr
        // Ничего не выделено
        dlgInput->lTipInput->clear();
        dlgInput->eInput->clear();
        //dlgInput->eInput->setEnabled(false);
        dlgInput->hide();
    } else {
        QGraphicsItem *it;
        it = ui->grafViewScene->scene()->selectedItems().at(0);
        //dlgInput->eInput->setEnabled(true);
        if (Node *n = dynamic_cast<Node *>(it)) {
            dlgInput->lTipInput->setText("Введите текст");
            dlgInput->eInput->setText(n->textContent());
        } else if (EdgeParent *e = dynamic_cast<EdgeParent *>(it)) {
            dlgInput->lTipInput->setText("Введи текст");
            dlgInput->eInput->setText(e->textContent());
        }

        //dlgInput->show();
        //dlgInput->raise();
        //dlgInput->activateWindow();
        dlgInput->exec();
    }
}

void FormGraph::onBtnCreateNodeClicked()
{
    ui->btnConnectNode->setChecked(false);

    int x, y;           // расположение вершины на сцене
    int numNode = 0;
    bool flFinding = false;     // флаг нахождения, при решение с каким состоянием создавать вершину
    Node *node;
    node = new Node(ui->grafViewScene);
    QString name = getInputText("Enter the name ");
    node->setName(name);
    numNode = node->id();
    // Определяет сколько вершин будут появлятся на одной оси У
    int nodeInRow = 6;
    x = - 2 * (2 * Node::Radius + 10) +
            ((!flFinding ? numNode : nodes.size()) % nodeInRow)
            * (2 * Node::Radius + 10);
    y = -100 + 2 * Node::Radius + 10 +
            ((!flFinding ? numNode : nodes.size()) / nodeInRow)
            * (2 * Node::Radius + 10);
    nodes.append(node);
    node->setPos(x, y);
    _source = nullptr;
    connFlag = 0;
    ui->lTip->setText("Добавлена вершина.");
    if (nodes.size()==9){
        ui->btnCreateNode->setEnabled(false);
    }
}
QString formgraph::getInputText(const QString &message){
    bool ok;
    QString input_text = QInputDialog::getText(0, "Input Dialog", message, QLineEdit::Normal, "", &ok);
    if (ok && !input_text.isEmpty()) {
        return input_text;
    }
    return "";
}

void FormGraph::onBtnConnectNodeClicked()
{
    if (ui->grafViewScene->scene()->selectedItems().size() > 0) {
        _source = dynamic_cast<Node *> (ui->grafViewScene->scene()->selectedItems().at(0));
        if (_source) {
            ui->grafViewScene->scene()->clearSelection();
            ui->lTip->setText("Выберите, куда будет проведена дуга.");
            connFlag = 2;
        } else {
            ui->lTip->clear();
            connFlag = 0;
        }
    }
    /* if (!_source) {
        if (connFlag == 0) {    // это условие не обязательное
            lTip->setText("Выберите вершину источник, потом получатель дуги");
            connFlag = 1;
            grafViewScene->scene()->clearSelection();
        }

    }*/
}

void FormGraph::onBtnDeleteClicked()
{
    if (ui->grafViewScene->scene()->selectedItems().size()) {
        _source = nullptr;
        connFlag = 0;
        auto i = ui->grafViewScene->scene()->selectedItems().at(0);
        if (Node* n = dynamic_cast<Node*>(i)) {
            foreach (auto e, n->edges()) {
                edges.removeAll(e);
            }
            if (n) {
                nodes.removeAll(n);
            } else {
                qDebug() << "dynamic_cast returned 0";
            }
            ui->btnCreateNode->setEnabled(true);
            ui->lTip->setText("Вершина удалена.");
        } else if (EdgeParent *e = dynamic_cast<EdgeParent*>(i)) {
            if (e) {
                edges.removeAll(e);
            } else {
                qDebug() << "dynamic_cast returned 0";
            }
            ui->lTip->setText("Дуга удалена.");

        } else {
            qDebug() << tr("I don't know what it is. type == %1").arg(i->type());
        }
        ui->grafViewScene->scene()->removeItem(i);
        delete i;
    }
}

void FormGraph::eInputTextChange()
{
    if(dlgInput->eInput->text().size() != 0 && dlgInput->eInput->hasAcceptableInput()) {
        dlgInput->btnApply->setEnabled(true);
    } else {
        dlgInput->btnApply->setEnabled(false);
    }
}

void FormGraph::onBtnApplyClicked()
{
    if (dlgInput->eInput->hasAcceptableInput()){
        if (ui->grafViewScene->scene()->selectedItems().size() != 1) {
            qDebug() << "grafViewScene->scene()->selectedItems().size() == "
                     << ui->grafViewScene->scene()->selectedItems().size();
            return;
        }
        auto it = ui->grafViewScene->scene()->selectedItems().at(0);
        NodeEdgeParent *nodeEdge = dynamic_cast<NodeEdgeParent*>(it);
        if (nodeEdge) {
            nodeEdge->setTextContent(dlgInput->eInput->text());
        } else { // if (it->type() == Edge::Type) {
            qDebug() << "It does not NodeEdgeParent";
        }
        dlgInput->hide();
    } else {
        dlgInput->lTipInput->setText("Неверный формат.");
    }
}

void FormGraph::sceneSelectionChanged()
{
    dlgInput->hide();
    QList<QGraphicsItem *> l = ui->grafViewScene->scene()->selectedItems();
    if (l.size() == 1) {
        ui->lTip->setText("Выделена вершина.");
        if (Node *node = dynamic_cast<Node *>(l.at(0))) {
            // Выделена вершина!
            ui->btnConnectNode->setEnabled(true);
            if (connFlag == 1) {
                // Назначен "Источник"
                _source = node;
                connFlag = 2;
                ui->lTip->setText("Выберите вершину куда будет проведена дуга.");
            } else if (connFlag == 2) {

                QString text = getInputText("Enter the weight");
                QString name = getInputText("Enter the name");
                
                // Нужно соединить с новой вершиной
                EdgeParent *e;
                if (_source == node) {
                    e = new EdgeCircle(_source);
                    e->setWeight(text.toInt());
                    e->setName(name);
                } else {
                    e = new Edge(_source, node);
                    e->setWeight(text.toInt());
                    e->setName(name);
                }
                edges.append(e);
                ui->btnConnectNode->setChecked(false);

                connFlag = 0;
                _source = nullptr;
            } else if (connFlag==3){

            }
        } else {
            // Выделена стрелка
            ui->lTip->setText("Выделена дуга.");
            ui->btnConnectNode->setEnabled(false);
            connFlag = false;
            _source = nullptr;
        }
        ui->btnDelete->setEnabled(true);

    } else if (l.size() > 1) {
        // Всегда должено быть выделено не более 1ого элемента
        qDebug() << "grafViewScene->scene()->selectedItems().size() == " << l.size();
    } else {
        // Пропало выделение (после удаления или нажатия на "Соединить")
        ui->lTip->setText("");
        ui->btnDelete->setEnabled(false);
        ui->btnConnectNode->setEnabled(false);
    }
}

void FormGraph::savePng(QString fileName) const
{
    QPixmap pixMap = QPixmap::grabWidget(ui->grafViewScene);
    pixMap.save(fileName);
}


bool FormGraph::saveGraph(QString fileName, bool jsonFormat) const
{
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject graphJson;
//    automat->writeToJson(graphJson);                            // automat
    ui->grafViewScene->writeToJson(graphJson);                  // scene
    QJsonArray jsonNodes;
    std::for_each(nodes.begin(), nodes.end(), [&] (Node *n) {
        QJsonObject json;
        n->writeToJson(json);
        jsonNodes.append(json); });
    graphJson["nodes"] = jsonNodes;                  // nodes
    QJsonArray jsonEdges;
    std::for_each(edges.begin(), edges.end(), [&] (EdgeParent *e) {
        QJsonObject json;
        e->writeToJson(json);
        jsonEdges.append(json); });
    graphJson["edges"] = jsonEdges;                  // edges
    QJsonDocument saveDoc(graphJson);
    saveFile.write(jsonFormat ?
                       saveDoc.toJson()
                     : saveDoc.toBinaryData());
    saveFile.close();
    return true;
}

void FormGraph::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        onBtnDeleteClicked();
        break;
    case Qt::Key_N:
    case 1058:
        onBtnCreateNodeClicked();
        break;
    case Qt::Key_C:
    case 1057:
        onBtnConnectNodeClicked();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

FormGraph *FormGraph::openGraph(QString fileName, bool jsonFormat) {
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open graph file.");
        return nullptr;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(jsonFormat ?
                              QJsonDocument::fromJson(saveData)
                            : QJsonDocument::fromBinaryData(saveData));
    const QJsonObject json = loadDoc.object();              // тут всё
    FormGraph *g = new FormGraph;
    g->ui->grafViewScene->readFromJson(json);                       // scene
    if (missKeys(json, QStringList { "nodes", "edges" })) {
        delete g;
        return nullptr;
    }
    QJsonArray jsonNodes = json["nodes"].toArray();
    std::for_each(jsonNodes.begin(), jsonNodes.end(), [&] (QJsonValue j) {
        Node *n = new Node(g->ui->grafViewScene);
        n->readFromJson(j.toObject());
        g->nodes.append(n);
    });
    QJsonArray jsonEdges = json["edges"].toArray();
    std::for_each(jsonEdges.begin(), jsonEdges.end(), [&] (QJsonValue j) {
        EdgeParent *e = EdgeParent::create(j.toObject(), g->ui->grafViewScene);
        g->edges.append(e);
    });
    g->ui->btnConnectNode->setEnabled(false);
    g->ui->btnDelete->setEnabled(false);
    return g;
}

void formgraph::initMatrix(){
    // Initialize the adjacency matrix with all elements set to zero
    int size = Node::idStatic();
    adjacencyMatrix.resize(size);
    for (int i = 0; i < size; ++i) {
        adjacencyMatrix[i].resize(size);
        for (int j = 0; j < size; ++j) {
            adjacencyMatrix[i][j] = 0;
        }
    }
    // Iterate over the edges and populate the adjacency matrix
    for (const EdgeParent* edge : edges) {
        int sourceIndex = nodes.indexOf(edge->sourceNode());
        int destIndex = nodes.indexOf(edge->destNode());
        if (sourceIndex != -1 && destIndex != -1) {
            adjacencyMatrix[sourceIndex][destIndex] = 1;
        }
    }
}
void formgraph::updateMatrixDisplay(){

    // Set the number of rows and columns in the table widget
    int numNodes = nodes.size();
    ui->tableWidget->setRowCount(numNodes);
    ui->tableWidget->setColumnCount(numNodes);

    // Set the header labels for rows and columns
   for (int i = 0; i < numNodes; ++i) {
        QTableWidgetItem* rowHeader = new QTableWidgetItem(nodes[i]->getName());
        QTableWidgetItem* colHeader = new QTableWidgetItem(nodes[i]->getName());
        ui->tableWidget->setVerticalHeaderItem(i, rowHeader);
        ui->tableWidget->setHorizontalHeaderItem(i, colHeader);
    }
    // Iterate over the adjacency matrix and set cell values
    for (int row = 0; row < numNodes; ++row) {
        for (int col = 0; col < numNodes; ++col) {
            int value = adjacencyMatrix[row][col];
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(value));
            ui->tableWidget->setItem(row, col, item);
        }
    }
}

void formgraph::on_displayMatrix_clicked()
{
    initMatrix();
    updateMatrixDisplay();
}



void formgraph::initMatrixIncidence() {
    int numNodes = Node::idStatic();
    int numEdges = edges.size();

    // Initialize the incidence matrix with all elements set to zero
    incidenceMatrix.resize(numNodes);
    for (int i = 0; i < numNodes; ++i) {
        incidenceMatrix[i].resize(numEdges);
        for (int j = 0; j < numEdges; ++j) {
            incidenceMatrix[i][j] = 0;
        }
    }

    // Iterate over the edges and populate the incidence matrix
    for (int edgeIndex = 0; edgeIndex < numEdges; ++edgeIndex) {
        const EdgeParent* edge = edges[edgeIndex];
        int sourceIndex = nodes.indexOf(edge->sourceNode());
        int destIndex = nodes.indexOf(edge->destNode());

        if (sourceIndex != -1 && destIndex != -1) {
            incidenceMatrix[sourceIndex][edgeIndex] = 1;
            incidenceMatrix[destIndex][edgeIndex] = -1;  // Set destination node entry to -1
        }
    }
}

void formgraph::updateMatrixDisplayIncidence() {
    // Clear the existing content in the table widget
    ui->tableWidget_2->clear();

    // Set the number of rows and columns in the table widget
    int numNodes = nodes.size();
    int numEdges = edges.size();
    ui->tableWidget_2->setRowCount(numNodes);
    ui->tableWidget_2->setColumnCount(numEdges);

    // Set the header labels for rows (nodes)
    for (int i = 0; i < numNodes; ++i) {
        QTableWidgetItem* rowHeader = new QTableWidgetItem(nodes[i]->getName());
        ui->tableWidget_2->setVerticalHeaderItem(i, rowHeader);
    }

    // Set the header labels for columns (edges)
    for (int j = 0; j < numEdges; ++j) {
        QTableWidgetItem* colHeader = new QTableWidgetItem(edges[j]->getName());
        ui->tableWidget_2->setHorizontalHeaderItem(j, colHeader);
    }

    // Iterate over the incidence matrix and set cell values
    for (int row = 0; row < numNodes; ++row) {
        for (int col = 0; col < numEdges; ++col) {
            int value = incidenceMatrix[row][col];
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(value));
            ui->tableWidget_2->setItem(row, col, item);
        }
    }
}

void formgraph::on_incidenceMatrix_clicked() {
    initMatrixIncidence();
    updateMatrixDisplayIncidence();
}
