#include "reminder.h"
#include <QDebug>
#include <QDateTime>

Reminder::Reminder(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Reminder::checkTasks);
    timer->start(60 * 1000); // 每分钟检查一次任务
}

// 添加任务到提醒系统
void Reminder::addTask(const Task &task) {
    monitoredTasks.append(task);
    qDebug() << "Task added to reminder:" << task.getTopic();
}

// 移除任务的提醒
void Reminder::removeTask(const QString &taskTopic) {
    for (int i = 0; i < monitoredTasks.size(); ++i) {
        if (monitoredTasks[i].getTopic() == taskTopic) {
            monitoredTasks.removeAt(i);
            qDebug() << "Task removed from reminder:" << taskTopic;
            return;
        }
    }
}

// 更新任务的提醒设置
void Reminder::updateTaskReminder(const Task &task) {
    removeTask(task.getTopic());
    addTask(task);
}

// 检查所有任务是否需要触发提醒
void Reminder::checkTasks() {
    QDateTime currentTime = QDateTime::currentDateTime();
    for (const Task &task : monitoredTasks) {
        if (task.isCompleted()) continue; // 跳过已完成任务

        QList<QDateTime> reminderTimes = calculateReminderTimes(task);
        for (const QDateTime &reminderTime : reminderTimes) {
            if (currentTime >= reminderTime && currentTime < reminderTime.addSecs(60)) {
                QString message = generateMessage(task, currentTime);
                emit reminderTriggered(task.getTopic(), message);
                break; // 每个任务只触发一次最近的提醒
            }
        }

        // 处理复习提醒（如果任务需要复习且已完成）
        if (task.needsReview() && task.isCompleted() &&
            currentTime >= task.getDueTime().addSecs(86400)) {  // 1天后 = 86400秒
            emit reminderTriggered(task.getTopic(), "复习提醒: " + task.getTopic());
        }
    }
}

// 根据优先级计算提醒时间点
QList<QDateTime> Reminder::calculateReminderTimes(const Task &task) const {
    QList<QDateTime> times;
    QDateTime dueTime = task.getDueTime();

    switch (task.getPriority()) {
    case Task::High:
        times << dueTime.addSecs(-86400)  // 提前1天 (60*60*24 = 86400秒)
              << dueTime.addSecs(-3600);  // 提前1小时 (60*60 = 3600秒)
        break;
    case Task::Medium:
        times << dueTime.addSecs(-10800); // 提前3小时 (60*60*3 = 10800秒)
        break;
    case Task::Low:
        times << dueTime.addSecs(-21600); // 提前6小时 (60*60*6 = 21600秒)
        break;
    }
    return times;
}

// 生成提醒消息
QString Reminder::generateMessage(const Task &task, const QDateTime &currentTime) const {
    int secsToDue = currentTime.secsTo(task.getDueTime());
    QString priorityStr;
    switch (task.getPriority()) {
    case Task::High: priorityStr = "高优先级"; break;
    case Task::Medium: priorityStr = "中优先级"; break;
    case Task::Low: priorityStr = "低优先级"; break;
    }

    return QString("【%1】%2 即将到期（剩余时间: %3分钟）").arg(priorityStr, task.getTopic(), QString::number(secsToDue / 60));
}
