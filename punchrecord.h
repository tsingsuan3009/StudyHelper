#ifndef PUNCHRECORD_H
#define PUNCHRECORD_H

#include <QObject>
#include <QMap>
#include <QDate>

class PunchRecord : public QObject {
    Q_OBJECT

public:
    explicit PunchRecord(QObject *parent = nullptr);

    // 记录打卡（任务完成）
    void recordPunch(const QString &taskTopic, const QDateTime &completionTime = QDateTime::currentDateTime());

    // 查询打卡记录
    bool hasPunched(const QString &taskTopic, const QDate &date) const;
    int getPunchCount(const QDate &date) const;
    int getContinuousPunchDays(const QString &taskTopic) const;

    // 数据导出
    QMap<QDate, int> getAllRecords() const; // 日期 -> 当日打卡数
    QMap<QDate, int> getPunchCounts(const QDate &start, const QDate &end) const;
    QMap<QString, QList<QDate>> getAllTaskRecords() const;
    QList<QDate> getTaskRecords(const QString &taskTopic) const; // 某任务的所有打卡日期

signals:
    void punchRecorded(const QString &taskTopic, const QDate &date);

private:
    QMap<QString, QList<QDate>> taskRecords; // 任务主题 -> 打卡日期列表
    QMap<QDate, int> dailyCounts;            // 每日打卡计数
};

#endif // PUNCHRECORD_H
