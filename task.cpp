#include "task.h"

Task::Task(const QString &topic, const QDateTime &dueTime, Priority priority, bool needsReview) : topic(topic), dueTime(dueTime), priority(priority), needsReviewFlag(needsReview), completed(false) {}

// 获取任务主题
QString Task::getTopic() const {
    return topic;
}

// 获取任务完成时间
QDateTime Task::getDueTime() const {
    return dueTime;
}

// 获取任务优先级
Task::Priority Task::getPriority() const {
    return priority;
}

// 是否需要复习提醒
bool Task::needsReview() const {
    return needsReviewFlag;
}

// 是否已完成
bool Task::isCompleted() const {
    return completed;
}

// 设置任务完成状态
void Task::setCompleted(bool completed) {
    this->completed = completed;
}

// 设置任务完成时间
void Task::setDueTime(const QDateTime &dueTime) {
    this->dueTime = dueTime;
}

// 设置任务优先级
void Task::setPriority(Priority priority) {
    this->priority = priority;
}
