#include "win.h"
#include "QDebug"
Win::Win(QWidget *parent) : QWidget(parent), _source(nullptr), connFlag(false)
{
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        {
            QHBoxLayout *layout = new QHBoxLayout(parent);
            btnCreateNode = new QPushButton("Добавить вершину", this);
            btnConnectNode = new QPushButton("Соединить", this);
            btnConnectNode->setEnabled(false);
            btnDelete = new QPushButton("Удалить", this);
            btnDelete->setEnabled(false);
            layout->addWidget(btnCreateNode);
            layout->addWidget(btnConnectNode);
            layout->addWidget(btnDelete);
            QPushButton * btn = new QPushButton("Save", this);
            layout->addWidget(btn);
            connect(btn, &QPushButton::clicked, this, &Win::sceneSave);
            mainLayout->addLayout(layout);
        }
        lNameGraf = new QLabel(this);
        lNameGraf->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(lNameGraf);
        lTip = new QLabel(this);
        mainLayout->addWidget(lTip);
        grafViewScene = new GraphWidget(this);
        mainLayout->addWidget(grafViewScene);
        {
            QGridLayout *layout = new QGridLayout(parent);
            lTipInput = new QLabel("Подсказки тут",this);
            layout->addWidget(lTipInput, 0, 0, 1, 2);
            eInput = new QLineEdit(this);
            layout->addWidget(eInput, 1, 0);
            btnApply = new QPushButton("Применить", this);
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
    automat = Automata::create(QApplication::arguments());
    if (automat->type != Automata::Type::FAIL_TYPE) {
        lNameGraf->setText((automat->type == Automata::Type::MILI ? "Автомат Мили": "Автомат Мура"));
        lTip->setText(QString("Вых файл: %1.png").arg(automat->outFile));
    }
}

Win::~Win() { }

void Win::designItem(QGraphicsItem *it)
{
    if (it->type() == QGraphicsItem::UserType + 1) { // Node
        Node *n = qgraphicsitem_cast<Node *>(it);
        eInput->setText(n->textInNode);
        if (automat->type == Automata::Type::MURA) {
            Automata::Mura *mur = dynamic_cast<Automata::Mura*>(automat);
            lTipInput->setText(
                tr("Шаблон ввода: aK/yL[,yM] или aK/-, где K от 0 до %1, L и M от 1 до %2")
                        .arg(mur->countA - 1).arg(mur->countY));
        }
    } else if (it->type() == QGraphicsItem::UserType + 2) { // Edge
        Edge *e = qgraphicsitem_cast<Edge *>(it);
        eInput->setText(e->textEdge);
        if (automat->type == Automata::Type::MURA) {
            Automata::Mura *mur = dynamic_cast<Automata::Mura*>(automat);
            lTipInput->setText(
                tr("Шаблон ввода: xK[,xM] или aK/-, где K и M от 1 до %1")
                        .arg(mur->countX));
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
            QString s = nodes[j]->textInNode.section(QRegExp("[^0-9]+"), 0, 0, QString::SectionSkipEmpty);
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
    Node *node = new Node(grafViewScene, QString("a%1/y1").arg(numNode));
    x = - 2 * (2 * Node::Radius + 10) + ((!flFinding ? numNode : nodes.size()) % 3) * (2 * Node::Radius + 10);
    y = -100 + 2 * Node::Radius + 10 + ((!flFinding ? numNode : nodes.size()) / 3) * (2 * Node::Radius + 10);
    nodes.append(node);
    node->setPos(x, y);
}

void Win::onBtnConnectNodeClicked()
{
    _source = qgraphicsitem_cast<Node *> (grafViewScene->scene()->selectedItems().at(0));
    if (_source) {
        connFlag = true;
        grafViewScene->scene()->clearSelection();
    } else {
        connFlag = false;
    }

}

void Win::onBtnDeleteClicked()
{
    _source = nullptr;
    connFlag = false;
    auto i = grafViewScene->scene()->selectedItems().at(0);
    if (i->type() == QGraphicsItem::UserType + 1) {         // Node
        Node* n = qgraphicsitem_cast<Node*>(i);
        if (n) {
            nodes.removeAll(n);
        } else {
            qDebug() << "qgraphicsitem_cast returned 0";
        }
    } else if (i->type() == QGraphicsItem::UserType + 2) {  // Edge
        Edge *e = qgraphicsitem_cast<Edge*>(i);
        if (e) {
            edges.removeAll(e);
        } else {
            qDebug() << "qgraphicsitem_cast returned 0";
        }
    } else {
        qDebug() << tr("I don't know what it is. type == %1").arg(i->type());
    }
    grafViewScene->scene()->removeItem(i);
    delete i;
}

void Win::sceneSelectionChanged()
{
    QList<QGraphicsItem *> l = grafViewScene->scene()->selectedItems();
    if (l.size() == 1) {
        Node *dest = qgraphicsitem_cast<Node *>(l.at(0));
        if (dest) {
            // Выделена вершина!
            if (connFlag) {
                // Нужно соединить с новой вершиной
                Edge *e = new Edge(_source, dest, "x1");
                edges.append(e);
                grafViewScene->scene()->addItem(e);
                connFlag = false;
                _source = nullptr;
                btnCreateNode->setEnabled(true);
            }
            btnConnectNode->setEnabled(true);
            btnDelete->setEnabled(true);
        } else {
            // Выделена стрелка
            connFlag = false;
            _source = nullptr;
            btnConnectNode->setEnabled(false);
            btnDelete->setEnabled(true);
        }
        designItem(l.at(0));
    } else if (l.size() > 1) {
        // Всегда должено быть выделено не более 1ого элемента
        qDebug() << "grafViewScene->scene()->selectedItems().size() == " << l.size();
    } else {
        // Пропало выделение (после удаления или нажатия на "Соединить")
        if (connFlag)
            btnCreateNode->setEnabled(false);
        else
            btnCreateNode->setEnabled(true);
        btnConnectNode->setEnabled(false);
        btnDelete->setEnabled(false);
    }
}

void Win::sceneSave()
{
    QPixmap pixMap = QPixmap::grabWidget(grafViewScene);
    pixMap.save(automat->outFile + ".png");
}
