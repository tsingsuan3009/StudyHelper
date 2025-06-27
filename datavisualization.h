#ifndef DATAVISUALIZATION_H
#define DATAVISUALIZATION_H

#include <QWidget>
#include <QtCharts>
#include <QBarSeries>
#include <QBarSet>
#include "punchrecord.h"
#include "taskmanager.h"

QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_END_NAMESPACE // 启用Qt Charts命名空间

class DataVisualization : public QWidget {
    Q_OBJECT

public:
    explicit DataVisualization(TaskManager *taskManager, PunchRecord *punchRecord, QWidget *parent = nullptr);
    void updateCharts(); // 刷新所有图表
    QDate getCurrentStartDate() const { return currentStartDate; }
    QDate getCurrentEndDate() const { return currentEndDate; }

public slots:
    void handleTaskCompleted();  // 新增专门处理任务完成的槽函数
    void updateTrendChartData(const QDate &startDate, const QDate &endDate);
    void updateAxisTicks(QDateTimeAxis *axis, const QDate &startDate, const QDate &endDate);
    void updateDateRangeLabel();

private:
    QPushButton *leftBtn;
    QPushButton *rightBtn;
    QLabel *dateRangeLabel;

    QDate currentStartDate;  // 记录当前显示的开始日期
    QDate currentEndDate;    // 记录当前显示的结束日期
    void setupNavigationButtons();  // 设置导航按钮功能

    // 数据源
    TaskManager *taskManager;
    PunchRecord *punchRecord;

    // 图表组件
    QChartView *completionRateChartView;    // 任务完成率饼图
    QChartView *dailyTaskTrendChartView;    // 每日任务趋势折线图
    QCalendarWidget *punchCalendar;         // 打卡日历

    // 初始化方法
    void adjustDateRange(int days);
    void initCompletionRateChart();
    void initDailyTaskTrendChart();
    void initPunchCalendar();
    void updateDateDisplay();
    void updateDateDisplay(QLabel *label);
};

#endif // DATAVISUALIZATION_H
