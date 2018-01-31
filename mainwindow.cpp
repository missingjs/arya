#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
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

    auto te = TaskEditor::instance();
    connect(te, &TaskEditor::modified, [=]() {
            this->setWindowModified(true);
    });
    connect(te, &TaskEditor::changed, [=](bool c) {
        this->setWindowModified(c);
    });
    connect(te, &TaskEditor::undoUpdate, this, &MainWindow::undoProp);
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
    auto te = TaskEditor::instance();
    te->redo();
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

void MainWindow::on_actionClose_triggered()
{
    auto te = TaskEditor::instance();
    if (preClose()) {
        te->clear();
        clearEditor();
    }
}

void MainWindow::clearEditor()
{
    setWindowTitle("Arya Editor");
    ui->taskTable->clearContents();
    ui->taskTable->setRowCount(0);
}

bool MainWindow::preClose()
{
    auto te = TaskEditor::instance();
    if (te->needSave()) {
        auto result = QMessageBox::question(this, "保存", "文件已修改，是否保存？", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (result == QMessageBox::Yes) {
            te->save();
        } else if (result == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::on_actionExit_triggered()
{
    if (preClose()) {
        QApplication::exit(0);
    }
}
