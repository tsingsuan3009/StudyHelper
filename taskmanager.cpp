#include "taskmanager.h"
#include <QDebug>

TaskManager::TaskManager(QObject *parent) : QObject(parent) {}

// 添加任务
void TaskManager::addTask(const Task &task) {
    tasks.append(task);
    emit tasksChanged(); // 通知UI更新
    emit taskAdded(task);
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
