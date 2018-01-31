#include "taskeditor.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

TaskEditor::TaskEditor(QObject *parent) : QObject(parent)
{

}

TaskEditor::~TaskEditor()
{
}

TaskEditor *TaskEditor::instance()
{
    static TaskEditor te;
    return &te;
}

void TaskEditor::resetByFile(const QString &path)
{
    itemMgr.reset();

    QFile data(path);
    if (data.open(QFile::ReadOnly)) {
        QTextStream out(&data);
        while (!out.atEnd()) {
            QString line = out.readLine();
            itemMgr.insert(parseLine(line));
        }
    }

    opList.clear();
    curPos = savePos = -1;
}

QList<int> TaskEditor::validTasks()
{
    return itemMgr.validTasks();
}

QString TaskEditor::value(int id, TaskItem::Field field) const
{
    return itemMgr.value(id, field);
}

void TaskEditor::update(int id, TaskItem::Field field, const QString &value)
{
    auto oldValue = itemMgr.value(id, field);
    if (itemMgr.update(id, field, value)) {
        auto op = Operation::upd();
        op->upd_setItemId(id);
        op->upd_setField(field);
        op->upd_setOldValue(oldValue);
        op->upd_setNewValue(value);
        addOp(op);
        emit modified(id, field, value);
    }
}

void TaskEditor::undo()
{
    if (curPos < 0) {
        return;
    }

    auto op = opList[curPos];
    switch (op->type) {
    case Operation::Type::UPDATE:
        undo_update(op);
        break;
    }
    --curPos;
}

TaskItem *TaskEditor::parseLine(const QString &line)
{
    TaskItem *item = nullptr;
    QRegularExpression re("^(.*)_(\\d+)_(\\d+[a-zA-Z]?)$");
    QString content = line.trimmed();
    if (content[0] == '#') {
        item = itemMgr.create(TaskItem::Type::COMMENT);
    } else if (content.isEmpty()) {
        item = itemMgr.create(TaskItem::Type::EMPTY);
    } else {
        item = itemMgr.create(TaskItem::Type::TASK);
        auto fields = content.split("=");
        QString fullName = fields[0];
        auto match = re.match(fullName);
        if (match.hasMatch()) {
            item->setPackName(match.captured(1));
            item->setPackVersion(match.captured(2).toInt());
            item->setPackChannel(match.captured(3));
        }

        item->setNewUserCount(fields[2].toInt());
        item->setRetentionFactor(fields[3]);
        item->setApkPath(fields[4]);
        item->setConfPath(fields[5]);
    }
    return item;
}

void TaskEditor::addOp(QSharedPointer<Operation> op)
{
    if (curPos == opList.size() - 1) {
        opList.append(op);
    } else {
        opList[curPos+1] = op;
    }
    ++curPos;
}

void TaskEditor::undo_update(QSharedPointer<Operation> op)
{
    int id = op->upd_itemId();
    auto field = op->upd_field();
    auto value = op->upd_oldValue();
    itemMgr.update(id, field, value);
    emit undoUpdate(id, field, value);
}

ItemManager::~ItemManager()
{
    reset();
}

TaskItem *ItemManager::create(TaskItem::Type type)
{
    int id = id_counter++;
    auto item = new TaskItem{id, type};
    nmap[id] = item;
    return item;
}

void ItemManager::insert(TaskItem *item)
{
    if (!head) {
        head = tail = item;
    } else {
        tail->next = item;
        item->prev = tail;
        tail = item;
    }
    ++node_count;
}

void ItemManager::insert(TaskItem *item, int next)
{
    if (!item) {
        return;
    }

    auto nxItem = nmap.value(next);
    if (!nxItem) {
        return;
    }

    auto pvItem = nxItem->prev;
    if (pvItem) {
        pvItem->next = item;
        item->prev = pvItem;
    }
    item->next = nxItem;
    nxItem->prev = item;

    ++node_count;
}

void ItemManager::reset()
{
    id_counter = 0;
    TaskItem *p = head, *t;
    while (p) {
        t = p->next;
        delete p;
        p = t;
    }
    head = tail = nullptr;
    nmap.clear();
}

QList<int> ItemManager::validTasks()
{
    QList<int> tasks;
    auto p = head;
    while (p) {
        if (p->isValidTask()) {
            tasks << p->id;
        }
        p = p->next;
    }
    return tasks;
}

QString ItemManager::value(int id, TaskItem::Field field) const
{
    auto item = nmap.value(id);
    return item ? item->values.value(field) : "";
}

bool ItemManager::update(int id, TaskItem::Field field, const QString &value)
{
    auto item = nmap.value(id);
    if (item) {
        item->values[field] = value;
        return true;
    } else {
        return false;
    }
}
