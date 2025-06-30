#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "taskmanager.h"
#include "punchrecord.h"
#include <QObject>
#include <QMap>

class Analysis : public QObject {
    Q_OBJECT

public:
    explicit Analysis(TaskManager *taskManager, PunchRecord *punchRecord, QObject *parent = nullptr);

    // 基础统计
    double getCompletionRate() const;                     // 总完成率(0.0~1.0)
    int getDailyAverageTasks() const;                     // 日均任务数
    QMap<QString, double> getPriorityCompletionRates() const; // 各优先级任务完成率

    // 趋势分析
    QMap<QDate, int> getDailyTaskCounts(int days = 30) const;    // 每日任务数（最近days天）
    QMap<QDate, int> getDailyCompletionCounts(int days = 30) const; // 每日完成数

    // 效率分析
    QList<int> getWeeklyCompletionTrend() const;          // 周完成趋势（最近8周）
    QString getEfficiencyAnalysis() const;                // 文本分析报告

signals:
    void analysisUpdated(); // 数据更新信号

public slots:
    void updateAnalysis();  // 手动触发更新分析

private:
    TaskManager *taskManager;
    PunchRecord *punchRecord;

};

#endif // ANALYSIS_H
