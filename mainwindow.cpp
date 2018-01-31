#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTableWidgetItem>

#include "utils.h"

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
    connect(TaskEditor::instance(), &TaskEditor::undoUpdate, this, &MainWindow::undoProp);
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

    imap.clear();
    for (int row = 0; row < tasks.size(); ++row) {
        int id = tasks[row];
        for (int i = 0; i < _columns.size(); ++i) {
            TaskItem::Field f = _columns[i];
            tb->setItem(row, i, new QTableWidgetItem(te->value(id, f)));
        }
        auto firstItem = tb->item(row, 0);
        firstItem->setData(ID_ROLE, id);
        imap[id] = firstItem;
    }

    connect(ui->taskTable, &QTableWidget::itemChanged, this, &MainWindow::handleItemChange);
}

void MainWindow::openFile(const QString &file)
{
    auto te = TaskEditor::instance();
    te->resetByFile(file);
    refreshTask();
    QString fileName = Utils::basename(file);
    setWindowTitle(QString("%1[*] - Arya Editor").arg(fileName));
}

void MainWindow::handleItemChange(QTableWidgetItem *item)
{
    int r = item->row(), c = item->column();
    int id = ui->taskTable->item(r, 0)->data(ID_ROLE).toInt();
    TaskEditor *te = TaskEditor::instance();
    te->update(id, _columns[c], item->text());
}

void MainWindow::on_actionRedo_triggered()
{

}

void MainWindow::undoProp(int taskId, TaskItem::Field field, const QString &value)
{
    auto firstItem = imap.value(taskId);
    if (!firstItem) {
        return;
    }

    int row = firstItem->row();
    int col = _columns.indexOf(field);
    if (col < 0) {
        return;
    }

    disconnect(ui->taskTable, &QTableWidget::itemChanged, this, &MainWindow::handleItemChange);
    ui->taskTable->item(row, col)->setText(value);
    connect(ui->taskTable, &QTableWidget::itemChanged, this, &MainWindow::handleItemChange);
}

void MainWindow::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, "打开任务文件");
    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

void MainWindow::on_actionUndo_triggered()
{
    auto te = TaskEditor::instance();
    te->undo();
}
