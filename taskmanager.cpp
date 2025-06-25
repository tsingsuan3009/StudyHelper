#include "taskmanager.h"
#include <QDebug>

TaskManager::TaskManager(QObject *parent) : QObject(parent) {
    initDatabase();
    loadTasksFromDatabase();  // 启动自动加载任务
}

// 添加任务
void TaskManager::addTask(const Task &task) {
    tasks.append(task);
    insertTaskToDatabase(task);
    emit tasksChanged(); // 通知UI更新
    emit taskAdded(task);
    emit tasksChanged();
}

// 删除任务
void TaskManager::removeTask(int index) {
    if (index >= 0 && index < tasks.size()) {
        tasks.removeAt(index);
        emit tasksChanged();
    } else {
        qWarning() << "Invalid task index:" << index;
    }
}

// 更新任务
void TaskManager::updateTask(int index, const Task &task) {
    if (index >= 0 && index < tasks.size()) {
        tasks[index] = task;
        emit tasksChanged();
    } else {
        qWarning() << "Invalid task index:" << index;
    }
}

// 获取任务列表
const QList<Task>& TaskManager::getTasks() const {
    return tasks;
}

// 标记任务完成状态
void TaskManager::markTaskCompleted(int index, bool completed) {
    if (index >= 0 && index < tasks.size()) {
        tasks[index].setCompleted(completed);
        updateTaskInDatabase(tasks[index]);
        emit tasksChanged();
        emit taskCompleted();
    }
}

// 加载系统推荐任务（示例）
void TaskManager::loadRecommendedTasks() {
    tasks.clear();
    tasks.append(Task("每日英语单词", QDateTime::currentDateTime().addDays(1), Task::Medium, true));
    tasks.append(Task("编程练习", QDateTime::currentDateTime().addDays(2), Task::High, false));
    emit tasksChanged();
}

void TaskManager::initDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("tasks.db");  // SQLite 文件保存在程序目录下

    if (!db.open()) {
        qDebug() << "数据库打开失败:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS tasks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "topic TEXT, "
               "dueDate TEXT, "
               "priority INTEGER, "
               "isCompleted INTEGER, "
               "needReview INTEGER)");
}

void TaskManager::loadTasksFromDatabase() {
    tasks.clear();

    QSqlQuery query("SELECT topic, dueDate, priority, isCompleted, needReview FROM tasks");
    while (query.next()) {
        QString topic = query.value(0).toString();

        QDateTime due = QDateTime::fromString(query.value(1).toString(), "yyyy-MM-dd HH:mm:ss");

        int priorityInt = query.value(2).toInt();
        Task::Priority priority = static_cast<Task::Priority>(priorityInt);

        bool completed = query.value(3).toInt();
        bool review = query.value(4).toInt();

        Task task(topic, due, priority, review);
        task.setCompleted(completed);
        tasks.append(task);
    }

    emit tasksChanged();  // 通知界面刷新
}

void TaskManager::insertTaskToDatabase(const Task &task) {
    QSqlQuery query;
    query.prepare("INSERT INTO tasks (topic, dueDate, priority, isCompleted, needReview) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(task.getTopic());
    query.addBindValue(task.getDueTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.addBindValue(task.getPriority());
    query.addBindValue(task.isCompleted());
    query.addBindValue(task.needsReview());
    query.exec();
}

void TaskManager::updateTaskInDatabase(const Task &task) {
    QSqlQuery query;
    query.prepare("UPDATE tasks SET isCompleted = ? WHERE topic = ?");
    query.addBindValue(task.isCompleted());
    query.addBindValue(task.getTopic());
    query.exec();
}

void TaskManager::deleteTaskFromDatabase(const Task &task) {
    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE topic = ?");
    query.addBindValue(task.getTopic());
    query.exec();
}
