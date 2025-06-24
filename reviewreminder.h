#ifndef REVIEWREMINDER_H
#define REVIEWREMINDER_H

#include "reminder.h"
#include <QMap>

class ReviewReminder : public Reminder {
    Q_OBJECT

public:
    explicit ReviewReminder(QObject *parent = nullptr);

    // 添加需要复习的任务（覆盖基类方法）
    void addTask(const Task &task) ;

    // 设置复习间隔（默认使用艾宾浩斯曲线：1,2,4,7,15天）
    void setReviewIntervals(const QList<int> &intervals);

private slots:
    void checkReviewTasks(); // 专门检查复习提醒

private:
    QMap<QString, QList<QDateTime>> reviewSchedules; // 任务主题 -> 复习时间列表
    QList<int> intervals; // 复习间隔（天）
};

#endif // REVIEWREMINDER_H
