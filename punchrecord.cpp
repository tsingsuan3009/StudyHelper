#include "punchrecord.h"
#include <QDebug>

PunchRecord::PunchRecord(QObject *parent) : QObject(parent) {
    if (!QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("studyhelper.db");
        db.open();
    } else {
        db = QSqlDatabase::database();
    }
    initDB();
}

void PunchRecord::initDB() {
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS punch_records ("
               "date TEXT PRIMARY KEY, "
               "count INTEGER DEFAULT 0)");
}

void PunchRecord::recordPunch(const QDate &date) {
    QString dateStr = date.toString("yyyy-MM-dd");

    // 查询是否已有该日期记录
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT count FROM punch_records WHERE date = ?");
    checkQuery.addBindValue(dateStr);
    if (checkQuery.exec() && checkQuery.next()) {
        int currentCount = checkQuery.value(0).toInt();
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE punch_records SET count = ? WHERE date = ?");
        updateQuery.addBindValue(currentCount + 1);
        updateQuery.addBindValue(dateStr);
        if (!updateQuery.exec()) {
            qDebug() << "更新打卡失败:" << updateQuery.lastError().text();
        }
    } else {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO punch_records (date, count) VALUES (?, 1)");
        insertQuery.addBindValue(dateStr);
        if (!insertQuery.exec()) {
            qDebug() << "插入打卡失败:" << insertQuery.lastError().text();
        }
    }
}

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
    QString dateStr = date.toString("yyyy-MM-dd");
    QSqlQuery query;
    query.prepare("SELECT count FROM punch_records WHERE date = ?");
    query.addBindValue(dateStr);
    query.exec();
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
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

QMap<QDate, int> PunchRecord::getPunchHistory(const QDate &start, const QDate &end) const {
    QMap<QDate, int> result;
    QSqlQuery query;
    query.prepare("SELECT date, count FROM punch_records WHERE date BETWEEN ? AND ?");
    query.addBindValue(start.toString("yyyy-MM-dd"));
    query.addBindValue(end.toString("yyyy-MM-dd"));
    query.exec();
    while (query.next()) {
        QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        int count = query.value(1).toInt();
        result[date] = count;
    }
    return result;
}
