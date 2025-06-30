#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"
#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class TaskManager : public QObject {
    Q_OBJECT

public:
    explicit TaskManager(QObject *parent = nullptr);

    // 任务管理
    void addTask(const Task &task);
    void removeTask(int index);
    void updateTask(int index, const Task &task);
    const QList<Task>& getTasks() const;

    // 任务状态操作
    void markTaskCompleted(int index, bool completed);

    // 系统推荐任务（示例）
    void loadRecommendedTasks();

    void reloadTasks();

signals:
    void tasksChanged(); // 任务列表变更信号
    void taskAdded(const Task &task);
    void taskCompleted();

private:
    QList<Task> tasks; // 存储所有任务
    QSqlDatabase db;
    void initDatabase();  // 新增：初始化 SQLite 表结构
    void loadTasksFromDatabase();  // 新增：从数据库读取任务
    void insertTaskToDatabase(const Task &task);
    void updateTaskInDatabase(const Task &task);
    void deleteTaskFromDatabase(const Task &task);

};

#endif // TASKMANAGER_H
