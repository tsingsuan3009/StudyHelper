#include "systemrecommendation.h"
#include <QRandomGenerator>

SystemRecommendation::SystemRecommendation(PunchRecord *record) : punchRecord(record) {
    initTopics();
}

void SystemRecommendation::initTopics() {
    predefinedTopics.clear();

    // 高校常见学科
    predefinedTopics << "高等数学" << "线性代数" << "概率论与数理统计"
                     << "大学物理" << "大学英语" << "思政与道法";

    // 专业课程（计算机示例，可按需扩展）
    predefinedTopics << "数据结构" << "操作系统" << "计算机网络" << "数据库原理"
                     << "编译原理" << "软件工程" << "人工智能基础";

    // 编程与项目实践
    predefinedTopics << "C++ 项目开发" << "Python 爬虫练习" << "Java 课程设计"
                     << "前端页面设计" << "Qt 桌面开发" << "LeetCode 刷题";

    // 考试类准备
    predefinedTopics << "四六级词汇记忆" << "雅思听力训练" << "考研数学强化"
                     << "考研政治分析题" << "GRE 填空训练" << "面试算法整理";

    // 能力提升类
    predefinedTopics << "时间管理技巧学习" << "自律习惯养成" << "效率工具使用"
                     << "学习日志撰写" << "周计划回顾" << "每日总结";

    // 语言 & 表达
    predefinedTopics << "英语演讲练习" << "日语五十音复习" << "发音纠正"
                     << "写作提升训练" << "阅读技巧总结";

    // 补充提升
    predefinedTopics << "复习笔记整理" << "错题本回顾" << "复习计划检查"
                     << "课程笔记再理解" << "知识点关联图绘制";
}

QList<Task> SystemRecommendation::generateRecommendations(int count) {
    QList<Task> result;
    QSet<QString> usedTopics;

    QDateTime now = QDateTime::currentDateTime();
    int attempts = 0;

    while (result.size() < count && attempts < 30) {
        int index = QRandomGenerator::global()->bounded(predefinedTopics.size());
        QString topic = predefinedTopics.at(index);

        if (usedTopics.contains(topic)) {
            attempts++;
            continue;
        }

        // 随机推迟 2~7 天
        int daysLater = QRandomGenerator::global()->bounded(2, 8);
        QDateTime due = now.addDays(daysLater);

        // 检查数据库中是否已有同样任务
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM tasks WHERE topic = ? AND dueDate = ?");
        query.addBindValue(topic);
        query.addBindValue(due.toString("yyyy-MM-dd HH:mm:ss"));
        if (query.exec() && query.next() && query.value(0).toInt() > 0) {
            attempts++;
            continue;  // 存在重复，跳过
        }

        Task t(topic, due, Task::Medium, true);
        result.append(t);
        usedTopics.insert(topic);
        attempts++;
    }

    return result;
}
