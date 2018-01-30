#ifndef TASKEDITOR_H
#define TASKEDITOR_H

#include <QMap>
#include <QObject>

struct TaskItem
{
    enum class Type {
        EMPTY = 0,
        COMMENT = 1,
        TASK = 2
    };

    enum class Field {
        // [0]
        PACK_NAME,
        PACK_VERSION,
        PACK_CHANNEL,

        // [2]
        NEW_USER_COUNT,

        // [3]
        RETENTION_FACTOR,

        // [4]
        APK_PATH,

        // [5]
        CONF_PATH
    };

    int id;

    Type type;

    QMap<Field, QString> values;

    bool isValidTask() const { return type == Type::TASK; }

    QString packName() const { return values[Field::PACK_NAME]; }
    void setPackName(const QString &s) { values[Field::PACK_NAME] = s; }

    int packVersion() const { return values[Field::PACK_VERSION].toInt(); }
    void setPackVersion(int v) { values[Field::PACK_VERSION] = QString::number(v); }

    QString packChannel() const { return values[Field::PACK_CHANNEL]; }
    void setPackChannel(const QString &s) { values[Field::PACK_CHANNEL] = s; }

    int newUserCount() const { return values[Field::NEW_USER_COUNT].toInt(); }
    void setNewUserCount(int n) { values[Field::NEW_USER_COUNT] = QString::number(n); }

    QString retentionFactor() const { return values[Field::RETENTION_FACTOR]; }
    void setRetentionFactor(const QString &s) { values[Field::RETENTION_FACTOR] = s; }

    QString apkPath() const { return values[Field::APK_PATH]; }
    void setApkPath(const QString &s) { values[Field::APK_PATH] = s; }

    QString confPath() const { return values[Field::CONF_PATH]; }
    void setConfPath(const QString &s) { values[Field::CONF_PATH] = s; }
};

struct Mod
{
    int id;
    TaskItem::Field field;
    QString oldValue;
    QString newValue;
};

class TaskEditor : public QObject
{
    Q_OBJECT

    explicit TaskEditor(QObject *parent = nullptr);

public:
    ~TaskEditor();

    static TaskEditor *instance();

    void addLine(const QString &line);

    QList<TaskItem*> tasks() const;

    QList<int> validTasks() const;

    QString value(int id, TaskItem::Field field) const;

    void update(int id, TaskItem::Field field, const QString &value);

signals:
    void modified(int id, TaskItem::Field field, const QString &value);

public slots:

private:
    TaskItem *parseLine(const QString &line);

    bool idValid(int id) const;

private:
    QList<TaskItem*> itemList;

    QList<Mod> modList;
    int modPos;

};

#endif // TASKEDITOR_H
