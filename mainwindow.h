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

    void refreshTask();

    void openFile(const QString &file);

private slots:
    void handleItemChange(QTableWidgetItem *item);

    void on_actionRedo_triggered();

    void undoProp(int taskId, TaskItem::Field field, const QString &value);

    void on_actionOpen_triggered();

    void on_actionUndo_triggered();

    void on_actionClose_triggered();

    void on_actionExit_triggered();

private:

    void clearEditor();

    bool preClose();

private:
    Ui::MainWindow *ui;

    // task item id to table item in first column
    QMap<int, QTableWidgetItem*> imap;
};

#endif // MAINWINDOW_H
