#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->taskTable, &QTableWidget::itemChanged, this, &MainWindow::handleItemChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setContent(const QList<TaskItem *> &items)
{
    auto tb = ui->taskTable;
    tb->clearContents();
    tb->setRowCount(items.size());
    const QString apkpfx("/home/panda/panda/share/apk/");
    const QString cfgpfx("/home/panda/panda/share/emulator/appconfig/");
    for (int i = 0; i < items.size(); ++i) {
        auto item = items[i];
        tb->setItem(i, 0, new QTableWidgetItem(item->packName));
        tb->setItem(i, 1, new QTableWidgetItem(QString::number(item->version)));
        tb->setItem(i, 2, new QTableWidgetItem(item->channel));
        tb->setItem(i, 3, new QTableWidgetItem(QString::number(item->newUserCount)));
        tb->setItem(i, 4, new QTableWidgetItem(item->retentionFactor));
        if (item->apkPath.startsWith(apkpfx)) {
            tb->setItem(i, 5, new QTableWidgetItem(item->apkPath.mid(apkpfx.size())));
        } else {
            tb->setItem(i, 5, new QTableWidgetItem(item->apkPath));
        }
        if (item->confPath.startsWith(cfgpfx)) {
            tb->setItem(i, 6, new QTableWidgetItem(item->confPath.mid(cfgpfx.size())));
        } else {
            tb->setItem(i, 6, new QTableWidgetItem(item->confPath));
        }
    }
}

void MainWindow::handleItemChange(QTableWidgetItem *item)
{
//    item->row(), item->column();
//    ui->taskTable->setco
}
