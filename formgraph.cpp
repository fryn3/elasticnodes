#include "formgraph.h"
#include "ui_formgraph.h"

#include <QJsonDocument>

FormGraph::FormGraph(QWidget *parent) :
    QWidget(parent), automat(nullptr),
    ui(new Ui::FormGraph),
    _source(nullptr), connFlag(false)
{
    ui->setupUi(this);

    dlgInput = new DlgInput();

    connect(ui->btnCreateNode, &QPushButton::clicked, this, &FormGraph::onBtnCreateNodeClicked);
    connect(ui->btnConnectNode, &QPushButton::clicked, this, &FormGraph::onBtnConnectNodeClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &FormGraph::onBtnDeleteClicked);
    connect(ui->grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &FormGraph::sceneSelectionChanged);
    connect(dlgInput->btnApply, &QPushButton::clicked, this, &FormGraph::onBtnApplyClicked);
    connect(ui->btnCheck, &QPushButton::clicked, this, &FormGraph::onBtnCheckClicked);
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

bool FormGraph::CreateAutomat(QStringList source)
{
    automat = new Automata::Universal(source);
    if (!automat->f || !automat->t){
        qDebug() << "Win::CreateAutomat : !automat";
        return false;
    }
    ui->lNameGraf->setText((automat->t->type() == Automata::Mili::Type ? "Автомат Мили": "Автомат Мура"));
    ui->btnCheck->setEnabled(true);
    ui->lTip->setText(QString("Вых файл: OutFile.png"));
    return true;
}

void FormGraph::CreateAutomat(Automata::Universal *_automat)
{
    automat = _automat;
    ui->lNameGraf->setText((automat->t->type() == Automata::Mili::Type ? "Автомат Мили": "Автомат Мура"));
    ui->lTip->setText(QString("Вых файл: OutFile.png"));
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
            if (automat) {
                dlgInput->eInput->setValidator(new QRegExpValidator(automat->t->regExpNode()));
                dlgInput->lTipInput->setText(automat->t->tipNode());
            } else {
                dlgInput->lTipInput->setText("Введите текст");
            }
            dlgInput->eInput->setText(n->textContent());
        } else if (EdgeParent *e = dynamic_cast<EdgeParent *>(it)) {
            if (automat) {
                dlgInput->eInput->setValidator(new QRegExpValidator(automat->t->regExpEdge()));
                dlgInput->lTipInput->setText(automat->t->tipEdge());
            } else {
                dlgInput->lTipInput->setText("Введи текст");
            }
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
    if (automat) {
        for (auto i = 0; i < automat->f->countA; i++) {
            flFinding = false;
            for (int j = 0; j < nodes.size(); j++) {
                QString s = nodes[j]->textContent().section(QRegExp("[^0-9]+"), 0, 0, QString::SectionSkipEmpty);
                if (s.toInt() == i) {
                    flFinding = true;
                    break;
                }
            }
            if (!flFinding || i == automat->f->countA - 1) {
                numNode = i;
                break;
            }
        }
        QString nodeText = (automat->t->type() == Automata::Mura::Type ? "a%1/y1" : "a%1");
        node = new Node(ui->grafViewScene, QString(nodeText).arg(numNode));
    } else {
        node = new Node(ui->grafViewScene);
        numNode = node->id();
    }
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
            if (n) {
                nodes.removeAll(n);
            } else {
                qDebug() << "dynamic_cast returned 0";
            }
            if (nodes.size()==0){
                ui->btnCheck->setEnabled(false);
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

void FormGraph::onBtnCheckClicked()
{
    if (automat) {
        if (automat->f->format() == Automata::Table::Format) {
            checkedTable();
        } else if (automat->f->format() == Automata::Matrix::Format) {
            checkedMatrixStr();
        }
    }
}

void FormGraph::checkedTable()
{
    QMessageBox msgBox;
    if (nodes.size() != automat->f->countA) {
        msgBox.setText("Error.");
        msgBox.exec();
        return;
    }
    int flFail = 0;
    QVector<QMultiMap<QString, int> > ch;
    if (automat->t->type() == Automata::Mura::Type) {
        ch.resize(nodes.size());
        // Проверка на очередность вершин
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (listNums(nodes.at(j)->textContent())
                        .at(0).toInt() == i) {
                    break;
                } else if (j == nodes.size() - 1) {
                    flFail = 1;
                }
            }
        }
        for (int i = 0; i < nodes.size(); i++) {
            // "a2/y1,y3" or "a1/-"
            QStringList nums = listNums(nodes.at(i)->textContent());
            if (nums.at(0).toInt() >= automat->f->countA) {
                flFail = 2;
                break;
            }
            if (nums.size() == 1) { // "a1/-"
                ch[(nums.at(0).toInt())].insert("y", 0);
            } else {
                for (int j = 1; j < nums.size(); j++) {
                    ch[(nums.at(0).toInt())].insert("y", nums.at(j).toInt());
                }
            }
        }

        for (int i = 0; i < edges.size(); i++) {
            if (flFail) {
                break;
            }
            int sourceNum = listNums(edges.at(i)->sourceNode()->textContent())
                    .at(0).toInt();
            int destNum = listNums(edges.at(i)->destNode()->textContent())
                    .at(0).toInt();
            // "x1" or "x2,x4"
            QStringList nums = listNums(edges.at(i)->textContent());
            for (int j = 0; j < nums.size(); j++) {
                ch[(sourceNum)].insert(QString("x%1").arg(nums.at(j)), destNum);
            }
        }
        // Заполнение пропусков
        for(int i = 0; i < ch.size(); i++) {
            for (int j = 1; j < automat->f->countX + 1; j++) {
                bool flMissKey = true;
                for (auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1").arg(j), 0);
                }
            }
        }
    } else if (automat->t->type() == Automata::Mili::Type) {
        ch.resize(nodes.size());
        // Проверка на очередность вершин
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes.at(i)->textContent()
                        .remove(0, 1).toInt() == i) {
                    break;
                } else if (j == nodes.size() - 1) {
                    flFail = 1;
                }
            }
        }

        for (int i = 0; i < edges.size(); i++) {
            if (flFail) {
                break;
            }
            int sourceNum = listNums(edges.at(i)->sourceNode()->textContent()).at(0).toInt();
            int destNum = listNums(edges.at(i)->destNode()->textContent()).at(0).toInt();
            // "x1/y1,y2" or "x2/-"
            QStringList nums = listNums(edges.at(i)->textContent());
            if (nums.at(0).toInt() > automat->f->countX) {
                flFail = 2;
                break;
            }
            ch[sourceNum].insert(tr("x%1a").arg(nums.at(0)), destNum);
            if (nums.size() == 1) {
                ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), 0);
            } else {
                for (int j = 1; j < nums.size(); j++) {
                    if (nums.at(j).toInt() > automat->f->countY) {
                        flFail = 3;
                        break;
                    }
                    ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), nums.at(j).toInt());
                }
            }
        }
        // Заполнение пустот
        for (int i = 0; i < automat->f->countA; i++) {
            for (int j = 1; j < automat->f->countX; j++) {
                bool flMissKey = true;
                for(auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1a").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1a").arg(j), 0);
                }
                flMissKey = true;
                for(auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1y").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1y").arg(j), 0);
                }
            }
        }
    }
    if (!flFail && automat->check(ch)) {
        QPixmap pixMap = QPixmap::grabWidget(ui->grafViewScene);
        pixMap.save("OutFile.png");
        msgBox.setText("Всё верно! Результат сохранён!");
        msgBox.exec();
    } else {
        msgBox.setText(tr("Неверно!\nКод ошибки: #%1").arg(flFail));
        msgBox.exec();
    }
}

void FormGraph::checkedMatrixStr()
{
    QStringList result;
    result.append(QString("%1").arg(automat->t->type()));
    result.append(QString("%1").arg(automat->f->format()));
    result.append(QString("%1").arg(nodes.size()));
    if (automat->t->type() == Automata::Mili::Type) {
        result.append(QString("%1").arg(nodes.size()));
    } else if (automat->t->type() == Automata::Mura::Type) {
        result.append(QString("%1").arg(nodes.size() + 1));
        // Заполнение строчки с y2,y3 или y1
        for (int aN = 0; aN < nodes.size(); aN++) {
            bool flError = true;
            foreach (auto node, nodes) {
                if (listNums(node->textContent())
                        .at(0).toInt() == aN) {
                    result.append(node->textContent().split("/").at(1));
                    flError = false;
                    break;
                }
            }
            if (flError) {
                qDebug() << "Вершины идут не по порядку: а0, а1, а4, а5";
                QMessageBox msgBox;
                msgBox.setText("Error.");
                msgBox.exec();
                return;
            }
        }
    }

    for (auto sourceN = 0; sourceN < nodes.size(); sourceN++) {
        for (auto destN = 0; destN < nodes.size(); destN++) {
            QStringList duplicateEdge;  // условно, объеденяются через запятую
            foreach (auto edge, edges) {
                if ((listNums(edge->sourceNode()->textContent())
                        .at(0).toInt() == sourceN)
                        && (listNums(edge->destNode()->textContent())
                            .at(0).toInt() == destN)) {
                    duplicateEdge.append(edge->textContent());
                }
            }
            if (duplicateEdge.size() == 0) {
                if (automat->t->type() == Automata::Mili::Type) {
                    result.append("-/-");
                } else if (automat->t->type() == Automata::Mura::Type) {
                    result.append("-");
                }
            } else {
                result.append(duplicateEdge.join(","));
            }
        }
    }
    if (automat->check(result)) {
        savePng("OutFile.png");
        QMessageBox msgBox;
        msgBox.setText("Всё верно! Результат сохранён!");
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Неверно!");
        msgBox.exec();
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
                // Нужно соединить с новой вершиной
                EdgeParent *e;
                if (automat) {
                    if (_source == node) {
                        e = new EdgeCircle(_source, (automat->t->type() == Automata::Mura::Type ? "x1" : "x1/y1"));
                    } else {
                        e = new Edge(_source, node, (automat->t->type() == Automata::Mura::Type ? "x1" : "x1/y1"));
                    }
                } else {
                    if (_source == node) {
                        e = new EdgeCircle(_source);
                    } else {
                        e = new Edge(_source, node);
                    }
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
    automat->writeToJson(graphJson);                            // automat
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
    case Qt::Key_Enter:
    case 16777220:
        onBtnCheckClicked();
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
    g->CreateAutomat(Automata::Universal::readFromJson(json));      // Automat
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

    return g;
}
