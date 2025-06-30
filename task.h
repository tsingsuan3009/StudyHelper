#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>

class Task {
public:
    // 任务优先级枚举
    enum Priority {
        Low,
        Medium,
        High
    };

    // 构造函数
    Task(const QString &topic, const QDateTime &dueTime, Priority priority, bool needsReview);

    // 获取任务属性
    QString getTopic() const;
    QDateTime getDueTime() const;
    Priority getPriority() const;
    bool needsReview() const;
    bool isCompleted() const;

    // 设置任务状态
    void setCompleted(bool completed);
    void setDueTime(const QDateTime &dueTime);
    void setPriority(Priority priority);

private:
    QString topic;          // 任务主题
    QDateTime dueTime;      // 完成时间
    Priority priority;      // 优先级
    bool needsReviewFlag;   // 是否需要复习提醒
    bool completed;         // 是否已完成

};

#endif // TASK_H
