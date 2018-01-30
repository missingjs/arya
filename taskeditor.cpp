#include "taskeditor.h"

#include <QRegularExpression>

TaskEditor::TaskEditor(QObject *parent) : QObject(parent)
{

}

TaskEditor::~TaskEditor()
{
    qDeleteAll(itemList.begin(), itemList.end());
}

TaskEditor *TaskEditor::instance()
{
    static TaskEditor te;
    return &te;
}

QList<TaskItem *> TaskEditor::tasks() const
{
    QList<TaskItem *> items;
    for (auto i : itemList) {
        if (i->type == TaskItem::Type::TASK) {
            items << i;
        }
    }
    return items;
}

QList<int> TaskEditor::validTasks() const
{
    QList<int> tasks;
    for (auto i : itemList) {
        if (i->isValidTask()) {
            tasks << i->id;
        }
    }
    return tasks;
}

QString TaskEditor::value(int id, TaskItem::Field field) const
{
    if (idValid(id)) {
        return itemList[id]->values.value(field);
    } else {
        return "";
    }
}

void TaskEditor::update(int id, TaskItem::Field field, const QString &value)
{
    if (id < 0 || id >= itemList.size()) {
        return;
    }

    auto item = itemList[id];
    item->values[field] = value;

    emit modified(id, field, value);
}

TaskItem *TaskEditor::parseLine(const QString &line)
{
    QRegularExpression re("^(.*)_(\\d+)_(\\d+[a-zA-Z]?)$");
    TaskItem *item = new TaskItem;
    QString content = line.trimmed();
    if (content[0] == '#') {
        item->type = TaskItem::Type::COMMENT;
    } else if (content.isEmpty()) {
        item->type = TaskItem::Type::EMPTY;
    } else {
        item->type = TaskItem::Type::TASK;
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

bool TaskEditor::idValid(int id) const
{
    return id >= 0 && id < itemList.size();
}

void TaskEditor::addLine(const QString &line)
{
    TaskItem *item = parseLine(line);
    item->id = itemList.size();
    itemList.append(item);
}
