#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <formgraph.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    FormGraph *formGraph;
protected slots:
    void graphSave() const;
    void graphSaveAs();
    void graphOpen();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
