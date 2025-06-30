#ifndef SYSTEMRECOMMENDATION_H
#define SYSTEMRECOMMENDATION_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QSet>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include "task.h"
#include "punchrecord.h"

class SystemRecommendation {
public:
    SystemRecommendation(PunchRecord *record = nullptr);
    QList<Task> generateRecommendations(int count = 3);

private:
    PunchRecord *punchRecord;
    QStringList predefinedTopics;
    void initTopics();

};

#endif // SYSTEMRECOMMENDATION_H
