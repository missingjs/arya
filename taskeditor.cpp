#include "taskeditor.h"

#include <QRegularExpression>

TaskEditor::TaskEditor(QObject *parent) : QObject(parent)
{

}

TaskEditor::~TaskEditor()
{
    qDeleteAll(itemList.begin(), itemList.end());
}

QList<TaskItem *> TaskEditor::getItems() const
{
    return itemList;
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
            item->packName = match.captured(1);
            item->version = match.captured(2).toInt();
            item->channel = match.captured(3);
        }

        item->newUserCount = fields[2].toInt();
        item->retentionFactor = fields[3];
        item->apkPath = fields[4];
        item->confPath = fields[5];
    }
    return item;
}

void TaskEditor::addLine(const QString &line)
{
    TaskItem *item = parseLine(line);
    item->id = itemList.size();
    itemList.append(item);
}
