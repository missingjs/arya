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
    if (itemMgr.update(id, field, value)) {
        emit modified(id, field, value);
    }
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

void ItemManager::insert(int prev, TaskItem *item)
{
    auto pvItem = nmap.value(prev);
    if (!pvItem) {
        return;
    }

    auto nxItem = pvItem->next;
    pvItem->next = item;
    item->prev = pvItem;
    if (nxItem) {
        item->next = nxItem;
        nxItem->prev = item;
    }
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
