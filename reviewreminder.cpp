#include "reviewreminder.h"
#include <QDebug>

ReviewReminder::ReviewReminder(QObject *parent) : Reminder(parent) {
    // 默认复习间隔（艾宾浩斯遗忘曲线）
    intervals = {1, 2, 4, 7, 15};
    // 单独定时器检查复习任务（每6小时一次）
    QTimer *reviewTimer = new QTimer(this);
    connect(reviewTimer, &QTimer::timeout, this, &ReviewReminder::checkReviewTasks);
    reviewTimer->start(6 * 3600 * 1000); // 6小时
}

void ReviewReminder::addTask(const Task &task) {
    if (task.needsReview()) {
        // 生成复习时间点
        QList<QDateTime> reviewTimes;
        QDateTime dueTime = task.getDueTime();
        for (int interval : intervals) {
            reviewTimes.append(dueTime.addSecs(interval * 86400)); // 转换为秒
        }
        reviewSchedules.insert(task.getTopic(), reviewTimes);
        qDebug() << "复习任务已安排:" << task.getTopic();
    }
    // 仍调用基类处理普通提醒
    Reminder::addTask(task);
}

void ReviewReminder::setReviewIntervals(const QList<int> &intervals) {
    this->intervals = intervals;
}

void ReviewReminder::checkReviewTasks() {
    QDateTime now = QDateTime::currentDateTime();
    for (auto it = reviewSchedules.begin(); it != reviewSchedules.end(); ++it) {
        const QString &topic = it.key();
        for (const QDateTime &reviewTime : it.value()) {
            if (now >= reviewTime && now < reviewTime.addSecs(3600)) { // 1小时窗口期
                emit reminderTriggered(topic,
                                       QString("复习提醒【%1】\n建议现在复习以巩固记忆").arg(topic));
                break;
            }
        }
    }
}
