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
    query.exec("CREATE TABLE IF NOT EXISTS punch_detail ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "date TEXT, "
               "topic TEXT)");
}

void PunchRecord::addPunchDetail(const QString &taskTopic, const QDate &date){
    const QString dateStr = date.toString("yyyy-MM-dd");

    /* 更新 punch_records（计数） -------------------------- */
    QSqlQuery check;
    check.prepare("SELECT count FROM punch_records WHERE date=?");
    check.addBindValue(dateStr);
    if (check.exec() && check.next()) {
        int cur = check.value(0).toInt();
        QSqlQuery up;
        up.prepare("UPDATE punch_records SET count=? WHERE date=?");
        up.addBindValue(cur + 1);
        up.addBindValue(dateStr);
        up.exec();
    } else {
        QSqlQuery ins;
        ins.prepare("INSERT INTO punch_records(date,count) VALUES(?,1)");
        ins.addBindValue(dateStr);
        ins.exec();
    }

    /* 插入 punch_detail（明细） -------------------------- */
    QSqlQuery det;
    det.prepare("INSERT INTO punch_detail(date,topic) VALUES(?,?)");
    det.addBindValue(dateStr);
    det.addBindValue(taskTopic);
    det.exec();

    // 内存同步（用于快速高亮）
    dailyCounts[date] += 1;
    taskRecords[taskTopic].append(date);

    // 通知界面
    emit punchRecorded(taskTopic, date);
}

QStringList PunchRecord::getTasksOfDate(const QDate &date) const{
    QStringList list;
    QSqlQuery q;
    q.prepare("SELECT topic FROM punch_detail WHERE date=?");
    q.addBindValue(date.toString("yyyy-MM-dd"));
    if (q.exec()){
        while (q.next())  list << q.value(0).toString();
    }
    return list;
}

bool PunchRecord::hasPunched(const QString &taskTopic, const QDate &date) const {
    return taskRecords.value(taskTopic).contains(date);
}

int PunchRecord::getPunchCount(const QDate &date) const {
    QString dateStr = date.toString("yyyy-MM-dd");
    QSqlQuery query;
    query.prepare("SELECT count FROM punch_records WHERE date = ?");
    query.addBindValue(dateStr);
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
