#include "win.h"
#include "QDebug"
Win::Win(QWidget *parent) : QWidget(parent), _source(nullptr), connFlag(false)
{
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        lNameGraf = new QLabel(this);
        lNameGraf->setAlignment(Qt::AlignCenter);
        lNameGraf->setFont(QFont("Times", 20, QFont::Bold));
        mainLayout->addWidget(lNameGraf);
        {
            QHBoxLayout *layout = new QHBoxLayout(parent);
            btnCreateNode = new QPushButton("◯", this);
            btnCreateNode->setFont(QFont("Times", 30, QFont::Bold));
            btnCreateNode->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
            btnConnectNode = new QPushButton("⤺", this);
            btnConnectNode->setFont(QFont("Times", 30, QFont::Bold));
            btnConnectNode->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
            btnDelete = new QPushButton("Удалить", this);
            btnDelete->setFont(QFont("Times", 20, QFont::Bold));
            btnDelete->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
            btnDelete->setEnabled(false);
            layout->addWidget(btnCreateNode);
            layout->addWidget(btnConnectNode);
            layout->addWidget(btnDelete);
            mainLayout->addLayout(layout);
        }
        lTip = new QLabel(this);
        lTip->setFont(QFont("Times", 15));
        mainLayout->addWidget(lTip);
        grafViewScene = new GraphWidget(this);
        mainLayout->addWidget(grafViewScene);
        {
            QGridLayout *layout = new QGridLayout(parent);
            lTipInput = new QLabel("Подсказки тут",this);
            layout->addWidget(lTipInput, 0, 0, 1, 2);
            eInput = new QLineEdit(this);
            eInput->setEnabled(false);
            layout->addWidget(eInput, 1, 0);
            btnApply = new QPushButton("Применить", this);
            btnApply->setEnabled(false);
            layout->addWidget(btnApply, 1, 1);
            btnCheck = new QPushButton("Проверить!", this);
            btnCheck->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
            layout->addWidget(btnCheck, 0, 2, 2, 1);
            mainLayout->addLayout(layout);
        }
    }
    connect(btnCreateNode, &QPushButton::clicked, this, &Win::onBtnCreateNodeClicked);
    connect(btnConnectNode, &QPushButton::clicked, this, &Win::onBtnConnectNodeClicked);
    connect(btnDelete, &QPushButton::clicked, this, &Win::onBtnDeleteClicked);
    connect(grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &Win::sceneSelectionChanged);
    connect(eInput, &QLineEdit::textChanged, this, &Win::eInputTextChange);
    connect(btnApply, &QPushButton::clicked, this, &Win::onBtnApplyClicked);
    connect(btnCheck, &QPushButton::clicked, this, &Win::onBtnCheckClicked);
    automat = Automata::create(QApplication::arguments());
    if (automat->type() != Automata::Abstract::Type) {
        lNameGraf->setText((automat->type() == Automata::Mili::Type? "Автомат Мили": "Автомат Мура"));
        lTip->setText(QString("Вых файл: %1.png").arg(automat->outFile));
    }
}

Win::~Win() { }

void Win::designItem(QGraphicsItem *it)
{
    if (!it) {  // nullptr
        // Ничего не выделено
        lTipInput->clear();
        eInput->clear();
        eInput->setEnabled(false);
    } else {
        eInput->setEnabled(true);
        if (it->type() == Node::Type) {
            Node *n = qgraphicsitem_cast<Node *>(it);
            eInput->setText(n->textContent());
            eInput->setValidator(new QRegExpValidator(automat->regExpNode()));
            lTipInput->setText(automat->tipNode());
        } else if (it->type() == Edge::Type) {
            Edge *e = qgraphicsitem_cast<Edge *>(it);
            eInput->setText(e->textContent());
            eInput->setValidator(new QRegExpValidator(automat->regExpEdge()));
            lTipInput->setText(automat->tipEdge());
        }
    }
}

void Win::onBtnCreateNodeClicked() {
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
    lTip->setText("Вершина добавлена");
}

void Win::onBtnConnectNodeClicked()
{
    if (grafViewScene->scene()->selectedItems().size() > 0) {
        _source = qgraphicsitem_cast<Node *> (grafViewScene->scene()->selectedItems().at(0));
        if (_source) {
            lTip->setText("Выберите вершину куда будет проведена дуга");
            connFlag = 2;
            grafViewScene->scene()->clearSelection();
        } else {
            lTip->clear();
            connFlag = 0;
        }
    }
    if (!_source) {
        if (connFlag == 0) {    // это условие не обязательное
            lTip->setText("Выберите вершину источник, потом получатель дуги");
            connFlag = 1;
            grafViewScene->scene()->clearSelection();
        }

    }
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
        lTip->setText("Вершина удалена");
    } else if (i->type() == Edge::Type) {
        Edge *e = qgraphicsitem_cast<Edge*>(i);
        if (e) {
            edges.removeAll(e);
        } else {
            qDebug() << "qgraphicsitem_cast returned 0";
        }
        lTip->setText("Дуга удалена");
    } else {
        qDebug() << tr("I don't know what it is. type == %1").arg(i->type());
    }
    grafViewScene->scene()->removeItem(i);
    delete i;
}

void Win::eInputTextChange()
{
    if(eInput->hasAcceptableInput())
        btnApply->setEnabled(true);
    else
        btnApply->setEnabled(false);
}

void Win::onBtnApplyClicked()
{
    if (grafViewScene->scene()->selectedItems().size() != 1) {
        qDebug() << "grafViewScene->scene()->selectedItems().size() == "
               << grafViewScene->scene()->selectedItems().size();
        return;
    }
    auto it = grafViewScene->scene()->selectedItems().at(0);
    NodeEdgeParent *nodeEdge = dynamic_cast<NodeEdgeParent*>(it);
    if (nodeEdge) {
        nodeEdge->setTextContent(eInput->text());
    } else { // if (it->type() == Edge::Type) {
        qDebug() << "It does not NodeEdgeParent";
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
                if (nodes.at(i)->textContent()
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
            if (nums.size() == 1) { // "a1/-"
                if (nums.at(0).toInt() >= automat->countA) {
                    flFail = 2;
                    break;
                }
                ch[(nums.at(0).toInt())].insert("y", 0);
                continue;
            }
            for (int j = 1; j < nums.size(); j++) {
                ch[(nums.at(0).toInt())].insert("y", nums.at(j).toInt());
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
                ch[(sourceNum)].insert(tr("x%1").arg(nums.at(j)), destNum);
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
        msgBox.setText("Success! Result saved!");
        msgBox.exec();
    } else {
        msgBox.setText(tr("Error!\nFail #%1").arg(flFail));
        msgBox.exec();
    }
}

void Win::sceneSelectionChanged()
{
    QList<QGraphicsItem *> l = grafViewScene->scene()->selectedItems();
    if (l.size() == 1) {
        lTip->setText("Выделена вершина. ");
        Node *node = qgraphicsitem_cast<Node *>(l.at(0));
        if (node) {
            // Выделена вершина!
            if (connFlag == 1) {
                // Назначен "Источник"
                _source = node;
                connFlag = 2;
                lTip->setText("Выберите вершину куда будет проведена дуга");
            } else if (connFlag == 2) {
                // Нужно соединить с новой вершиной
                // Проверка на повторное соединение
                bool miss = false;
                for (auto edg : _source->edges()) {
                    if (edg->sourceNode() == _source && edg->destNode() == node) {
                        miss = true;
                        lTip->setText("Попытка повторного соединения");
                        break;
                    }
                }
                if (!miss) {
                    Edge *e = new Edge(_source, node, (automat->type() == Automata::Mura::Type ? "x1" : "x1/y1"));
                    edges.append(e);
                    grafViewScene->scene()->addItem(e);
                }
                connFlag = 0;
                _source = nullptr;
            }
        } else {
            // Выделена стрелка
            lTip->setText("Выделена дуга.");
            connFlag = false;
            _source = nullptr;
        }
        btnDelete->setEnabled(true);
        designItem(l.at(0));
    } else if (l.size() > 1) {
        // Всегда должено быть выделено не более 1ого элемента
        qDebug() << "grafViewScene->scene()->selectedItems().size() == " << l.size();
    } else {
        // Пропало выделение (после удаления или нажатия на "Соединить")
        designItem(nullptr);
        btnDelete->setEnabled(false);
    }
}

void Win::sceneSave()
{
    QPixmap pixMap = QPixmap::grabWidget(grafViewScene);
    pixMap.save(automat->outFile + ".png");
}
