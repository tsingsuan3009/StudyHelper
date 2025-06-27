#include "analysis.h"
#include <QDate>
#include <algorithm>

// Analysis 构造函数，接收任务管理器和打卡记录对象作为参数
Analysis::Analysis(TaskManager *taskManager, PunchRecord *punchRecord, QObject *parent)
    : QObject(parent), taskManager(taskManager), punchRecord(punchRecord) {}

// 计算总任务完成率 (0.0~1.0)
double Analysis::getCompletionRate() const {
    const auto &tasks = taskManager->getTasks(); // 获取任务列表
    if (tasks.isEmpty()) return 0.0;             // 若无任务，完成率为 0

    // 统计已完成任务数量
    int completed = std::count_if(tasks.begin(), tasks.end(),
                                  [](const Task &t) { return t.isCompleted(); });

    // 返回完成数除以总任务数
    return static_cast<double>(completed) / tasks.size();
}

// 计算日均任务数（最近30天）
int Analysis::getDailyAverageTasks() const {
    auto dailyCounts = punchRecord->getAllRecords(); // 获取全部打卡记录
    if (dailyCounts.isEmpty()) return 0;

    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-29); // 30天范围

    int total = 0;
    for (const auto &date : dailyCounts.keys()) {
        if (date >= startDate && date <= endDate) {
            total += dailyCounts[date]; // 累加有效日期范围内的打卡数量
        }
    }
    return total / 30; // 返回日均数量
}

// 按优先级统计完成率
QMap<QString, double> Analysis::getPriorityCompletionRates() const {
    QMap<QString, double> rates;
    QMap<QString, int> totalCount, completedCount;

    for (const Task &task : taskManager->getTasks()) {
        QString priority;
        switch (task.getPriority()) {
        case Task::High: priority = "高"; break;
        case Task::Medium: priority = "中"; break;
        case Task::Low: priority = "低"; break;
        }
        totalCount[priority]++;
        if (task.isCompleted()) completedCount[priority]++;
    }

    for (const QString &key : totalCount.keys()) {
        rates[key] = totalCount[key] ? static_cast<double>(completedCount[key]) / totalCount[key] : 0.0;
    }
    return rates;
}

// 获取每日任务数量趋势
QMap<QDate, int> Analysis::getDailyTaskCounts(int days) const {
    QMap<QDate, int> result;
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-days + 1);

    for (QDate date = startDate; date <= endDate; date = date.addDays(1)) {
        result[date] = punchRecord->getPunchCount(date);
    }
    return result;
}

// 获取每周完成趋势（最近8周）
QList<int> Analysis::getWeeklyCompletionTrend() const {
    QList<int> weeklyTrend;
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-7 * 8 + 1); // 8周范围

    int currentWeekCount = 0;
    for (QDate date = startDate; date <= endDate; date = date.addDays(1)) {
        currentWeekCount += punchRecord->getPunchCount(date);
        if (date.dayOfWeek() == Qt::Sunday || date == endDate) {
            weeklyTrend.append(currentWeekCount);
            currentWeekCount = 0;
        }
    }
    return weeklyTrend;
}

// 生成文本分析报告
QString Analysis::getEfficiencyAnalysis() const {
    double rate = getCompletionRate();
    int avgTasks = getDailyAverageTasks();

    QString report;
    report += QString("📊 学习分析报告\n\n");
    report += QString("• 任务完成率: %1%\n").arg(rate * 100, 0, 'f', 1);
    report += QString("• 日均完成任务: %1个\n").arg(avgTasks);

    auto weeklyTrend = getWeeklyCompletionTrend();
    if (!weeklyTrend.isEmpty()) {
        int change = weeklyTrend.last() - weeklyTrend.first();
        report += QString("• 最近8周趋势: %1%2\n")
                      .arg(change >= 0 ? "↑" : "↓")
                      .arg(abs(change));
    }
    return report;
}

void Analysis::updateAnalysis() {
    emit analysisUpdated();
}
