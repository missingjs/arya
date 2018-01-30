#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "taskeditor.h"

namespace Ui {
class MainWindow;
}

class QTableWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setContent(const QList<TaskItem*> &items);

private slots:
    void handleItemChange(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
