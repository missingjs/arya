#ifndef TASKEDITOR_H
#define TASKEDITOR_H

#include <QObject>

struct TaskItem
{
    enum class Type {
        EMPTY = 0,
        COMMENT = 1,
        TASK = 2
    };

    int id;

    Type type;

    // [0]
    QString packName;
    int version;
    QString channel;

    // [1]
    // no use

    // [2]
    int newUserCount;

    // [3]
    QString retentionFactor;

    // [4]
    QString apkPath;

    // [5]
    QString confPath;
};

class TaskEditor : public QObject
{
    Q_OBJECT
public:
    explicit TaskEditor(QObject *parent = nullptr);

    ~TaskEditor();

    void addLine(const QString &line);

    QList<TaskItem*> getItems() const;

    QList<TaskItem*> tasks() const;

    void update(int id, const QString &field, const QString &value);

signals:

public slots:

private:
    TaskItem *parseLine(const QString &line);

private:
    QList<TaskItem*> itemList;
//    QList<QString>
};

#endif // TASKEDITOR_H
