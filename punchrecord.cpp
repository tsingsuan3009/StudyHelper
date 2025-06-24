#include "punchrecord.h"
#include <QDebug>

PunchRecord::PunchRecord(QObject *parent) : QObject(parent) {}

void PunchRecord::recordPunch(const QString &taskTopic, const QDateTime &completionTime) {
    QDate date = completionTime.date();

    // 更新任务打卡记录（避免重复）
    if (!taskRecords[taskTopic].contains(date)) {
        taskRecords[taskTopic].append(date);
        qDebug() << "打卡记录:" << taskTopic << date.toString("yyyy-MM-dd");
        dailyCounts[date]++;

        emit punchRecorded(taskTopic, date);
    }
}

bool PunchRecord::hasPunched(const QString &taskTopic, const QDate &date) const {
    return taskRecords.value(taskTopic).contains(date);
}

int PunchRecord::getPunchCount(const QDate &date) const {
    return dailyCounts.value(date, 0);
}

// 添加新方法获取日期范围内的打卡记录
QMap<QDate, int> PunchRecord::getPunchCounts(const QDate &start, const QDate &end) const {
    QMap<QDate, int> result;
    for (QDate date = start; date <= end; date = date.addDays(1)) {
        result[date] = dailyCounts.value(date, 0);
    }
    return result;
}

int PunchRecord::getContinuousPunchDays(const QString &taskTopic) const {
    QList<QDate> dates = taskRecords.value(taskTopic);
    if (dates.isEmpty()) return 0;

    // 按日期排序（最近的在前）
    std::sort(dates.begin(), dates.end(), std::greater<QDate>());

    int continuousDays = 1;
    for (int i = 1; i < dates.size(); ++i) {
        if (dates[i-1].daysTo(dates[i]) == -1) { // 相差1天
            continuousDays++;
        } else {
            break;
        }
    }
    return continuousDays;
}

QMap<QDate, int> PunchRecord::getAllRecords() const {
    return dailyCounts;
}

QList<QDate> PunchRecord::getTaskRecords(const QString &taskTopic) const {
    return taskRecords.value(taskTopic);
}
