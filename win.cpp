#include "win.h"
#include "QDebug"
Win::Win(QWidget *parent) : QWidget(parent), _source(nullptr), connFlag(false)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    {
        QVBoxLayout *rightLayout = new QVBoxLayout(parent);
        {
            QHBoxLayout *layout = new QHBoxLayout(parent);
            {
                btnCreateNode = new QPushButton("Добавить вершину", this);
                btnConnectNode = new QPushButton("Соединить", this);
                btnConnectNode->setEnabled(false);
                btnDelete = new QPushButton("Удалить", this);
                btnDelete->setEnabled(false);
                layout->addWidget(btnCreateNode);
                layout->addWidget(btnConnectNode);
                layout->addWidget(btnDelete);
            }
            rightLayout->addLayout(layout);
            lNameGraf = new QLabel(this);
            lNameGraf->setAlignment(Qt::AlignCenter);
            rightLayout->addWidget(lNameGraf);
            grafViewScene = new GraphWidget(this);
            rightLayout->addWidget(grafViewScene);
        }
        mainLayout->addLayout(rightLayout, 2);
    }
    connect(btnCreateNode, &QPushButton::clicked, this, &Win::onBtnCreateNodeClicked);
    connect(btnConnectNode, &QPushButton::clicked, this, &Win::onBtnConnectNodeClicked);
    connect(btnDelete, &QPushButton::clicked, this, &Win::onBtnDeleteClicked);
    connect(grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &Win::sceneSelectionChanged);
    if (QApplication::arguments().size() < 4) {
        qDebug() << "Table did not get";
    } else {
        typeAuto =   QApplication::arguments()[1].toInt();    // Мили или Мура
        int colums = QApplication::arguments()[2].toInt();    // и кол-во состояний
        int rows =   QApplication::arguments()[3].toInt();
        if (QApplication::arguments().size() != rows * colums + 4) {
            qDebug() << "Format table error";
        } else {
            table.resize(rows);
            for (int i = 0; i < rows; i++) {
                table[i].resize(colums);
                for(int j = 0; j < colums; j++) {
                    table[i][j] = QApplication::arguments()[i * colums + j + 4];
                }
            }
            lNameGraf->setText((typeAuto == TypeAutomat::Mili ? "Автомат Мили": "Автомат Мура"));
        }
    }
}

Win::~Win() { }

void Win::onBtnCreateNodeClicked() {
    static int x = 0, y = -100;
    Node *node = new Node(grafViewScene);
    switch ((Node::idStatic() - 1) % 3) {
    case 0:
        x -= 4 * Node::Radius + 20;
        y += 2 * Node::Radius + 10;
        break;
    case 1:
    case 2:
        x += 2 * Node::Radius + 10;
        break;
    }
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
    grafViewScene->scene()->removeItem(i);
    delete i;
}

void Win::sceneSelectionChanged()
{
    QList<QGraphicsItem *> l = grafViewScene->scene()->selectedItems();
    if (l.size() > 0) {
        Node *dest = qgraphicsitem_cast<Node *>(l.at(0));
        if (dest) {
            if (connFlag) {
                grafViewScene->scene()->addItem(new Edge(_source, dest));
                connFlag = false;
                _source = nullptr;
                btnCreateNode->setEnabled(true);
            }
            btnConnectNode->setEnabled(true);
            btnDelete->setEnabled(true);
        } else {
            connFlag = false;
            _source = nullptr;
            btnConnectNode->setEnabled(false);
            btnDelete->setEnabled(true);
        }        
    } else {
        if (connFlag)
            btnCreateNode->setEnabled(false);
        else
            btnCreateNode->setEnabled(true);
        btnConnectNode->setEnabled(false);
        btnDelete->setEnabled(false);
    }
}
