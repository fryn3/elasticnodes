#ifndef DLGINPUT_H
#define DLGINPUT_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>

class DlgInput : public QDialog
{
    Q_OBJECT
public:
    explicit DlgInput(QWidget *parent = nullptr);
    ~DlgInput() { }
    QLabel *lTipInput;   // Подсказки ввода
    QLineEdit *eInput;
    QPushButton *btnApply;   // Применить ввод (с проверкой корректности).
protected:

signals:

public slots:
};

#endif // DLGINPUT_H
