#ifndef REMINDER_H
#define REMINDER_H

#include "task.h"
#include <QObject>
#include <QTimer>
#include <QDateTime>

class Reminder : public QObject {
    Q_OBJECT

public:
    explicit Reminder(QObject *parent = nullptr);

    // 添加任务到提醒系统
    void addTask(const Task &task);
    // 移除任务的提醒
    void removeTask(const QString &taskTopic);
    // 更新任务的提醒时间
    void updateTaskReminder(const Task &task);

signals:
    // 提醒触发的信号（参数：任务主题、提醒消息）
    void reminderTriggered(const QString &topic, const QString &message);

private slots:
    // 检查任务是否需要提醒
    void checkTasks();

private:
    // 计算提醒时间（根据优先级和截止时间）
    QList<QDateTime> calculateReminderTimes(const Task &task) const;
    // 生成提醒消息
    QString generateMessage(const Task &task, const QDateTime &currentTime) const;

    QTimer *timer;                  // 定时器，定期检查任务
    QList<Task> monitoredTasks;     // 被监控的任务列表

};

#endif // REMINDER_H
