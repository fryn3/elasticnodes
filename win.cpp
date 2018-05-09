#include "win.h"
#include "QDebug"
Win::Win(QWidget *parent) : QWidget(parent), _source(nullptr), connFlag(false)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    {
        QVBoxLayout *leftLayout = new QVBoxLayout(parent);
        {
            QHBoxLayout *layout = new QHBoxLayout(parent);
            QLabel *label = new QLabel("Вариант", this);
            QLineEdit *edit = new QLineEdit("32", this);
            edit->setFont(QFont("Times", 22));
            btnSelectChoices = new QPushButton("Применить", this);
            layout->addWidget(label);
            layout->addWidget(edit);
            layout->addWidget(btnSelectChoices);
            leftLayout->addLayout(layout);
        }
        {
            QLabel *l = new QLabel("Автомат Мили");
            l->setAlignment(Qt::AlignHCenter);
            leftLayout->addWidget(l);
        }
        table = new QTableWidget(2, 3);
        QStringList hHeaderStr;
        for (int i = 0; i < table->columnCount(); i++) {
            hHeaderStr << tr("a%1").arg(QChar(0x2080 + i));
            table->setColumnWidth(i, (table->frameWidth() - 9) / table->columnCount());
        }
        table->setHorizontalHeaderLabels(hHeaderStr);
        QStringList vHeaderStr;
        for (int i = 0; i < table->rowCount(); i++) {
            vHeaderStr << tr("x%1").arg(QChar(0x2080 + i + 1));
        }
        table->setVerticalHeaderLabels(vHeaderStr);
        table->setMinimumWidth(330);
        for (int i = 0; i < table->rowCount(); i++) {
            for (int j = 0; j < table->columnCount(); j++) {
                QTableWidgetItem *pItem = new QTableWidgetItem(tr("a%1/y%2").arg(QChar(0x2080 + i + 1)).arg(QChar(0x2080 + i + 1)));
                pItem->setFont(QFont("Times", 13));
                table->setItem(i, j, pItem);
            }
        }
        table->setEnabled(false);
        leftLayout->addWidget(table);
        QSpacerItem *space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        leftLayout->addItem(space);
        mainLayout->addLayout(leftLayout, 1);
    }
    {
        QVBoxLayout *rightLayout = new QVBoxLayout(parent);
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
            rightLayout->addLayout(layout);
        }
        grafViewScene = new GraphWidget(this);
        rightLayout->addWidget(grafViewScene);
        mainLayout->addLayout(rightLayout, 2);
    }
    connect(btnCreateNode, &QPushButton::clicked, grafViewScene, &GraphWidget::addNode);
    connect(btnConnectNode, &QPushButton::clicked, this, &Win::onBtnConnectNodeClicked);
    connect(btnDelete, &QPushButton::clicked, this, &Win::onBtnDeleteClicked);
    connect(grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &Win::sceneSelectionChanged);
}

Win::~Win() { }


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
