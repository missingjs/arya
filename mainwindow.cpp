#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidgetItem>

namespace {

const int ID_ROLE = 10001;

using _fd = TaskItem::Field;
QList<TaskItem::Field> _columns = {
    TaskItem::Field::PACK_NAME,
    TaskItem::Field::PACK_VERSION,
    TaskItem::Field::PACK_CHANNEL,
    TaskItem::Field::NEW_USER_COUNT,
    TaskItem::Field::RETENTION_FACTOR,
    TaskItem::Field::APK_PATH,
    TaskItem::Field::CONF_PATH
};

} // anonymous namespace

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshTask()
{
    disconnect(ui->taskTable, &QTableWidget::itemChanged, this, nullptr);

    auto tb = ui->taskTable;
    tb->clearContents();

    auto te = TaskEditor::instance();
    auto tasks = te->validTasks();
    tb->setRowCount(tasks.size());

    for (int row = 0; row < tasks.size(); ++row) {
        int id = tasks[row];
        for (int i = 0; i < _columns.size(); ++i) {
            TaskItem::Field f = _columns[i];
            tb->setItem(row, i, new QTableWidgetItem(te->value(id, f)));
        }
        tb->item(row, 0)->setData(ID_ROLE, id);
    }

    connect(ui->taskTable, &QTableWidget::itemChanged, this, &MainWindow::handleItemChange);
}

void MainWindow::handleItemChange(QTableWidgetItem *item)
{
    int r = item->row(), c = item->column();
    int id = ui->taskTable->item(r, 0)->data(ID_ROLE).toInt();
    TaskEditor *te = TaskEditor::instance();
    te->update(id, _columns[c], item->text());
}
