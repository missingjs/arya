#ifndef TASKEDITOR_H
#define TASKEDITOR_H

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>

class ItemManager;

struct TaskItem
{
    enum class Type {
        DELETED = -1,
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

    const int id;

    const Type type;

    QMap<Field, QString> values;

    TaskItem *prev{nullptr};
    TaskItem *next{nullptr};

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

private:
    TaskItem(int id, Type type)
        : id{id}, type{type} {}

    friend class ItemManager;
};

Q_DECLARE_METATYPE(TaskItem::Field)

class ItemManager
{
public:
//    ItemManager();
    ~ItemManager();

    TaskItem *create(TaskItem::Type type);

    void insert(TaskItem *item);

    void insert(TaskItem *item, int next);

    void reset();

    QList<int> validTasks();

    QString value(int id, TaskItem::Field field) const;

    bool update(int id, TaskItem::Field field, const QString &value);

private:
    int id_counter{0};
    TaskItem *head{nullptr}, *tail{nullptr};
    int node_count{0};
    QMap<int, TaskItem*> nmap;
};


struct Operation
{
    enum class Type {
        INSERT,
        DELETE,
        UPDATE
    };

    const Type type;
    QVariantMap prop;

    Operation(Type type)
        : type{type}, prop{} {}

    static QSharedPointer<Operation> ins() { return QSharedPointer<Operation>::create(Type::INSERT); }
    static QSharedPointer<Operation> del() { return QSharedPointer<Operation>::create(Type::DELETE); }
    static QSharedPointer<Operation> upd() { return QSharedPointer<Operation>::create(Type::UPDATE); }

    // insert
    int ins_nextItemId() const { return prop["insert.next_id"].toInt(); }
    void ins_setNextItemId(int id) { prop["insert.next_id"] = id; }

    // delete
    int del_itemId() const { return prop["delete.item_id"].toInt(); }
    void del_setItemId(int id) { prop["delete.item_id"] = id; }

    // update
    int upd_itemId() const { return prop["update.item_id"].toInt(); }
    void upd_setItemId(int id) { prop["update.item_id"] = id; }

    TaskItem::Field upd_field() const { return prop["update.field"].value<TaskItem::Field>(); }
    void upd_setField(TaskItem::Field f) { prop["update.field"] = QVariant::fromValue(f); }

    QString upd_oldValue() const { return prop["update.old_value"].toString(); }
    void upd_setOldValue(const QString &s) { prop["update.old_value"] = s; }

    QString upd_newValue() const { return prop["update.new_value"].toString(); }
    void upd_setNewValue(const QString &s) { prop["update.new_value"] = s; }
};

class TaskEditor : public QObject
{
    Q_OBJECT

    explicit TaskEditor(QObject *parent = nullptr);

public:
    ~TaskEditor();

    static TaskEditor *instance();

    void resetByFile(const QString &path);

    QList<int> validTasks();

    QString value(int id, TaskItem::Field field) const;

    void update(int id, TaskItem::Field field, const QString &value);

    void undo();

signals:
    void modified(int id, TaskItem::Field field, const QString &value);

    void undoUpdate(int id, TaskItem::Field field, const QString &value);

public slots:

private:

    TaskItem *parseLine(const QString &line);

    void addOp(QSharedPointer<Operation> op);

    void undo_update(QSharedPointer<Operation> op);

private:
    ItemManager itemMgr;

    QList<QSharedPointer<Operation>> opList;
    int curPos{-1};
    int savePos{-1};

};

#endif // TASKEDITOR_H
