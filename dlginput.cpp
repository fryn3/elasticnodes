#include "dlginput.h"
#include <QDebug>
DlgInput::DlgInput(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    lTipInput = new QLabel("Подсказки тут",this);
    lTipInput->setFont(QFont("Times", 13));
    layout->addWidget(lTipInput, 0, 0, 1, 2);
    eInput = new QLineEdit(this);
    eInput->setEnabled(false);
    eInput->setFont(QFont("Times", 13));
    layout->addWidget(eInput, 1, 0);
    btnApply = new QPushButton("Применить", this);
    btnApply->setFont(QFont("Times", 13));
    btnApply->setEnabled(false);
    layout->addWidget(btnApply, 1, 1);
    setFixedSize(minimumSize());
}

