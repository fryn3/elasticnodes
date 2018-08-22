#include "win.h"
#include <QDebug>
#include <QApplication>
Win::Win(QWidget *parent) : QWidget(parent), _source(nullptr), connFlag(false)
{
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        lNameGraf = new QLabel(this);
        lNameGraf->setAlignment(Qt::AlignCenter);
        lNameGraf->setFont(QFont("Times", 12, QFont::Bold));
        mainLayout->addWidget(lNameGraf);
        {
            QHBoxLayout *layout = new QHBoxLayout(parent);
            btnCreateNode = new QPushButton("◯", this);
            btnCreateNode->setFont(QFont("Helvetica", 12));
            btnCreateNode->setMaximumWidth(31);
            btnCreateNode->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            btnConnectNode = new QPushButton("⤺", this);
            btnConnectNode->setFont(QFont("Helvetica", 10));
            btnConnectNode->setMaximumWidth(31);
            btnConnectNode->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            btnConnectNode->setCheckable(true);
            btnConnectNode->setEnabled(false);
            btnDelete = new QPushButton("Удалить", this);
            btnDelete->setFont(QFont("Helvetica", 10));
            btnDelete->setMaximumWidth(200);
            btnDelete->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            btnDelete->setEnabled(false);
            btnCheck = new QPushButton("Проверить", this);
            btnCheck->setFont(QFont("Helvetica", 10));
            btnCheck->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            btnCheck->setMaximumWidth(200);
            btnCheck->setEnabled(false);
            layout->addWidget(btnCreateNode);
            layout->addWidget(btnConnectNode);
            layout->addWidget(btnDelete);
            layout->addWidget(btnCheck);
            mainLayout->addLayout(layout);
        }
        lTip = new QLabel(this);
        lTip->setFont(QFont("Times", 12));
        mainLayout->addWidget(lTip);
        grafViewScene = new GraphWidget(this);
        mainLayout->addWidget(grafViewScene);
    }
    dlgInput = new DlgInput();

    connect(btnCreateNode, &QPushButton::clicked, this, &Win::onBtnCreateNodeClicked);
    connect(btnConnectNode, &QPushButton::clicked, this, &Win::onBtnConnectNodeClicked);
    connect(btnDelete, &QPushButton::clicked, this, &Win::onBtnDeleteClicked);
    connect(grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &Win::sceneSelectionChanged);
    //connect(dlgInput->eInput, &QLineEdit::editingFinished, this, &Win::eInputTextChange);
    connect(dlgInput->btnApply, &QPushButton::clicked, this, &Win::onBtnApplyClicked);
    connect(btnCheck, &QPushButton::clicked, this, &Win::onBtnCheckClicked);
    connect(grafViewScene, &GraphWidget::editItem, this, &Win::showInput);
}

    void Win::CreateAutomat (QStringList data)
    {
        automat = Automata::create(data);
        if (automat->type() != Automata::Abstract::Type)
        {
            lNameGraf->setText((automat->type() == Automata::Mili::Type? "Автомат Мили": "Автомат Мура"));
            lTip->setText(QString("Вых файл: %1.png").arg(automat->outFile));
        }
    }

Win::~Win() { }

void Win::closeEvent(QCloseEvent *event)
{
    dlgInput->close();
    // Важно! disconnect нужен для корректного выхода из приложения!
    disconnect(grafViewScene->scene(), 0, 0, 0);
}

void Win::showInput()
{

    //qDebug()<<"show input";
    if (grafViewScene->scene()->selectedItems().size() == 0) {  // nullptr
        // Ничего не выделено
        //qDebug()<<"show input 1";
        dlgInput->lTipInput->clear();
        dlgInput->eInput->clear();
        //dlgInput->eInput->setEnabled(false);
        dlgInput->hide();
    } else {
        //qDebug()<<"show input 2";
        QGraphicsItem *it;
        it = grafViewScene->scene()->selectedItems().at(0);
        //dlgInput->eInput->setEnabled(true);
        if (it->type() == Node::Type) {
            Node *n = qgraphicsitem_cast<Node *>(it);

            dlgInput->eInput->setValidator(new QRegExpValidator(automat->regExpNode()));
            dlgInput->eInput->setText(n->textContent());
            dlgInput->lTipInput->setText(automat->tipNode());
        } else if (it->type() == Edge::Type) {
            Edge *e = qgraphicsitem_cast<Edge *>(it);
            dlgInput->eInput->setValidator(new QRegExpValidator(automat->regExpEdge()));
            dlgInput->eInput->setText(e->textContent());
            dlgInput->lTipInput->setText(automat->tipEdge());
        }

        //dlgInput->show();
        //dlgInput->raise();
        //dlgInput->activateWindow();
        dlgInput->exec();
    }
}

void Win::onBtnCreateNodeClicked() {
    btnCheck->setEnabled(true);
    btnConnectNode->setChecked(false);



    int x, y;           // расположение вершины на сцене
    int numNode;
    bool flFinding;     // флаг нахождения, при решение с каким состоянием создавать вершину
    for (int i = 0; i < automat->countA; i++) {
        flFinding = false;
        for (int j = 0; j < nodes.size(); j++) {
            QString s = nodes[j]->textContent().section(QRegExp("[^0-9]+"), 0, 0, QString::SectionSkipEmpty);
            if (s.toInt() == i) {
                flFinding = true;
                break;
            }
        }
        if (!flFinding || i == automat->countA - 1) {
            numNode = i;
            break;
        }
    }
    QString nodeText = (automat->type() == Automata::Mura::Type ? "a%1/y1" : "a%1");
    Node *node = new Node(grafViewScene, QString(nodeText).arg(numNode));
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
    lTip->setText("Добавлена вершина.");
    if (nodes.size()==9){
        btnCreateNode->setEnabled(false);
    }

    qDebug()<<nodes<<endl;
}

void Win::onBtnConnectNodeClicked()
{
    if (grafViewScene->scene()->selectedItems().size() > 0) {
        _source = qgraphicsitem_cast<Node *> (grafViewScene->scene()->selectedItems().at(0));
        if (_source) {
            lTip->setText("Выберите, куда будет проведена дуга.");
            connFlag = 2;
            grafViewScene->scene()->clearSelection();
        } else {
            lTip->clear();
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

void Win::onBtnDeleteClicked()
{
    _source = nullptr;
    connFlag = 0;
    auto i = grafViewScene->scene()->selectedItems().at(0);
    if (i->type() == Node::Type) {
        Node* n = qgraphicsitem_cast<Node*>(i);
        if (n) {
            nodes.removeAll(n);
        } else {
            qDebug() << "qgraphicsitem_cast returned 0";
        }
        if (nodes.size()==0){
            btnCheck->setEnabled(false);
        }
        btnCreateNode->setEnabled(true);
        lTip->setText("Вершина удалена.");
    } else if (i->type() == Edge::Type) {
        Edge *e = qgraphicsitem_cast<Edge*>(i);
        if (e) {
            edges.removeAll(e);
        } else {
            qDebug() << "qgraphicsitem_cast returned 0";
        }
        lTip->setText("Дуга удалена.");

    } else {
        qDebug() << tr("I don't know what it is. type == %1").arg(i->type());
    }
    grafViewScene->scene()->removeItem(i);
    delete i;
}

void Win::eInputTextChange()
{
    qDebug()<<"FF";
    if(dlgInput->eInput->text().size()!=0 && dlgInput->eInput->hasAcceptableInput()) {
        dlgInput->btnApply->setEnabled(true);
    } else {
        dlgInput->btnApply->setEnabled(false);
    }
}

void Win::onBtnApplyClicked()
{
    if (dlgInput->eInput->hasAcceptableInput()){
    qDebug()<<"OK";
        if (grafViewScene->scene()->selectedItems().size() != 1) {
            qDebug() << "grafViewScene->scene()->selectedItems().size() == "
                   << grafViewScene->scene()->selectedItems().size();
            return;
        }
        auto it = grafViewScene->scene()->selectedItems().at(0);
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

void Win::onBtnCheckClicked()
{
    QMessageBox msgBox;
    if (nodes.size() != automat->countA) {
        msgBox.setText("Error.");
        msgBox.exec();
        return;
    }
    int flFail = 0;
    QVector<QMultiMap<QString, int> > ch;
    if (automat->type() == Automata::Mura::Type) {
        ch.resize(nodes.size());
        // Проверка на очередность вершин
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes.at(j)->textContent()
                        .split(QRegExp("[^0-9]"), QString::SkipEmptyParts)
                        .at(0).toInt() == i) {
                    break;
                } else if (j == nodes.size() - 1) {
                    flFail = 1;
                }
            }
        }
        for (int i = 0; i < nodes.size(); i++) {
            // "a2/y1,y3" or "a1/-"
            QStringList nums = nodes.at(i)->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            if (nums.at(0).toInt() >= automat->countA) {
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
            int sourceNum = edges.at(i)->sourceNode()->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts).at(0).toInt();
            int destNum = edges.at(i)->destNode()->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts).at(0).toInt();
            // "x1" or "x2,x4"
            QStringList nums = edges.at(i)->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            for (int j = 0; j < nums.size(); j++) {
                ch[(sourceNum)].insert(QString("x%1").arg(nums.at(j)), destNum);
            }
        }
        // Заполнение пропусков
        for(int i = 0; i < ch.size(); i++) {
            for (int j = 1; j < automat->countX + 1; j++) {
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
    } else if (automat->type() == Automata::Mili::Type) {
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
            int sourceNum = edges.at(i)->sourceNode()->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts).at(0).toInt();
            int destNum = edges.at(i)->destNode()->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts).at(0).toInt();
            // "x1/y1,y2" or "x2/-"
            QStringList nums = edges.at(i)->textContent()
                    .split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
            if (nums.at(0).toInt() > automat->countX) {
                flFail = 2;
                break;
            }
            ch[sourceNum].insert(tr("x%1a").arg(nums.at(0)), destNum);
            if (nums.size() == 1) {
                ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), 0);
            } else {
                for (int j = 1; j < nums.size(); j++) {
                    if (nums.at(j).toInt() > automat->countY) {
                        flFail = 3;
                        break;
                    }
                    ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), nums.at(j).toInt());
                }
            }
        }
        // Заполнение пустот
        for (int i = 0; i < automat->countA; i++) {
            for (int j = 1; j < automat->countX; j++) {
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
        QPixmap pixMap = QPixmap::grabWidget(grafViewScene);
        pixMap.save(automat->outFile + ".png");
        msgBox.setText("Всё верно! Результат сохранён!");
        msgBox.exec();
    } else {
        msgBox.setText(tr("Неверно!\nКод ошибки: #%1").arg(flFail));
        msgBox.exec();
    }
}

void Win::sceneSelectionChanged()
{
    dlgInput->hide();
    QList<QGraphicsItem *> l = grafViewScene->scene()->selectedItems();
    if (l.size() == 1) {
        lTip->setText("Выделена вершина. ");
        Node *node = qgraphicsitem_cast<Node *>(l.at(0));
        if (node) {
            // Выделена вершина!
            btnConnectNode->setEnabled(true);
            if (connFlag == 1) {
                // Назначен "Источник"
                _source = node;
                connFlag = 2;
                lTip->setText("Выберите вершину куда будет проведена дуга");
            } else if (connFlag == 2) {
                // Нужно соединить с новой вершиной
                // Проверка на повторное соединение
               bool miss = false;
               /*for (auto edg : _source->edges()) {
                    if (edg->sourceNode() == _source && edg->destNode() == node) {
                        miss = true;
                        lTip->setText("Попытка повторного соединения.");
                        break;
                    }
                }*/
                if (!miss) {
                    Edge *e = new Edge(_source, node, (automat->type() == Automata::Mura::Type ? "x1" : "x1/y1"));
                    edges.append(e);
                    grafViewScene->scene()->addItem(e);
                    btnConnectNode->setChecked(false);


                    //this->dr
                }
                connFlag = 0;
                _source = nullptr;
            } else if (connFlag==3){

            }
        } else {
            // Выделена стрелка
            lTip->setText("Выделена дуга.");
            btnConnectNode->setEnabled(false);
            connFlag = false;
            _source = nullptr;

            Edge *e = qgraphicsitem_cast<Edge *>(l.at(0));
            grafViewScene->startBezier(e);//

        }
        btnDelete->setEnabled(true);

    } else if (l.size() > 1) {
        // Всегда должено быть выделено не более 1ого элемента
        qDebug() << "grafViewScene->scene()->selectedItems().size() == " << l.size();
    } else {
        // Пропало выделение (после удаления или нажатия на "Соединить")
        btnDelete->setEnabled(false);
        btnConnectNode->setEnabled(false);
    }
}

void Win::sceneSave()
{
    QPixmap pixMap = QPixmap::grabWidget(grafViewScene);
    pixMap.save(automat->outFile + ".png");
}

void Win::mouseReleaseEvent(QMouseEvent *event){
    qDebug()<<"Released";
}

void Win::dropEvent(QDropEvent *event){
    qDebug()<<"Dropped";
}

void Win::loadAuto(int type,QStringList ylist,QStringList xlist,int size){}
