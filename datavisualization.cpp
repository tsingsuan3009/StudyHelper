#include "datavisualization.h"
#include "callout.h"
#include <QVBoxLayout>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QDate>

DataVisualization::DataVisualization(TaskManager *taskManager, PunchRecord *punchRecord, QWidget *parent)
    : QWidget(parent),
    taskManager(taskManager),
    punchRecord(punchRecord),
    leftBtn(nullptr),
    rightBtn(nullptr),
    dateRangeLabel(nullptr) {

    // 确保有图形场景
    QGraphicsScene *scene = new QGraphicsScene(this);

    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 初始化图表视图时确保有场景
    dailyTaskTrendChartView = new QChartView(this);
    dailyTaskTrendChartView->setRenderHint(QPainter::Antialiasing);
    if (!dailyTaskTrendChartView->scene()) {
        dailyTaskTrendChartView->setScene(new QGraphicsScene(this));
    }

    // 初始化图表
    initCompletionRateChart();
    initDailyTaskTrendChart();
    initPunchCalendar();

    // 添加到布局
    layout->addWidget(completionRateChartView);
    layout->addWidget(dailyTaskTrendChartView);
    layout->addWidget(punchCalendar);
}

void DataVisualization::initCompletionRateChart() {
    QChart *chart = new QChart();
    chart->setTitle("任务完成率");

    // 获取实际数据
    int completed = 0, total = taskManager->getTasks().size();
    for (const Task &task : taskManager->getTasks()) {
        if (task.isCompleted()) completed++;
    }

    QPieSeries *series = new QPieSeries();
    series->append("已完成", completed);
    series->append("未完成", total - completed);
    series->setLabelsVisible(true);

    chart->addSeries(series);
    completionRateChartView = new QChartView(chart);

    // 设置样式
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setMargins(QMargins(0, 0, 0, 0));

    // 设置饼图颜色
    series->slices().first()->setBrush(QColor(100, 149, 237));  // 已完成
    series->slices().last()->setBrush(QColor(255, 99, 71));     // 未完成
}

void DataVisualization::initDailyTaskTrendChart() {
    QChart *chart = new QChart();
    chart->setTitle("每日任务趋势 (最近7天)");

    // 创建系列
    QBarSeries *series = new QBarSeries();
    QBarSet *barSet = new QBarSet("完成任务数");
    *barSet << 0 << 0 << 0 << 0 << 0 << 0 << 0;
    series->append(barSet);
    chart->addSeries(series);

    // 初始化为 0 数据（防止初始不显示）
    QDate today = QDate::currentDate();
    QDate startDate = today.addDays(-6);

    //x轴
    QBarCategoryAxis *xAxis = new QBarCategoryAxis();
    QStringList categories;
    QDate start = QDate::currentDate().addDays(-6);
    for (int i = 0; i < 7; ++i) {
        categories << start.addDays(i).toString("MM-dd");
    }
    xAxis->append(categories);
    chart->addAxis(xAxis, Qt::AlignBottom);
    series->attachAxis(xAxis);

    //y轴
    QValueAxis *yAxis = new QValueAxis();
    yAxis->setRange(0, 5);
    yAxis->setTitleText("完成数量");
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(yAxis);

    chart->legend()->setVisible(false);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setMargins(QMargins(0, 0, 0, 0));
    xAxis->setLabelsFont(QFont("Arial", 8));
    xAxis->setGridLineVisible(true);
    xAxis->setGridLinePen(QPen(Qt::lightGray, 1, Qt::DotLine));
    yAxis->setGridLineVisible(true);
    yAxis->setGridLinePen(QPen(Qt::lightGray, 1, Qt::DotLine));

    dailyTaskTrendChartView = new QChartView(chart);
    dailyTaskTrendChartView->setRenderHint(QPainter::Antialiasing);
    dailyTaskTrendChartView->setRubberBand(QChartView::HorizontalRubberBand);
    dailyTaskTrendChartView->setInteractive(true);

    // 添加导航按钮
    QWidget *chartContainer = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(chartContainer);

    QPushButton *leftBtn = new QPushButton("◀");
    QPushButton *rightBtn = new QPushButton("▶");
    layout->addWidget(leftBtn);
    layout->addWidget(dailyTaskTrendChartView, 1);
    layout->addWidget(rightBtn);

    // 连接按钮信号
    connect(leftBtn, &QPushButton::clicked, this, [this]() {
        adjustDateRange(-7);
    });
    connect(rightBtn, &QPushButton::clicked, this, [this]() {
        adjustDateRange(7);
    });

    // 设置导航按钮
    setupNavigationButtons();

    // 首次加载数据
    currentEndDate = today;
    currentStartDate = startDate;

    // 初始按钮状态
    updateDateDisplay();

    // 创建悬浮提示框
    Callout *tooltip = new Callout(chart);
    chart->scene()->addItem(tooltip);

    updateTrendChartData(currentStartDate, currentEndDate);
}

void DataVisualization::adjustDateRange(int days) {
    // 计算新日期范围（保持7天跨度）
    QDate newStartDate = currentStartDate.addDays(days);
    QDate newEndDate = newStartDate.addDays(6);

    // 限制不能超过当前日期
    if (newEndDate > QDate::currentDate()) {
        newEndDate = QDate::currentDate();
        newStartDate = newEndDate.addDays(-6);
    }

    // 应用新范围
    currentStartDate = newStartDate;
    currentEndDate = newEndDate;
    updateTrendChartData(currentStartDate, currentEndDate);
    updateDateDisplay();  // 更新界面日期标签
}

void DataVisualization::updateAxisTicks(QDateTimeAxis *axis, const QDate &startDate, const QDate &endDate) {
    // 设置主要刻度（每天一个）
    axis->setTickCount(7);

    // 设置标签显示格式
    axis->setFormat("MM-dd");
}

void DataVisualization::updateTrendChartData(const QDate &startDate, const QDate &endDate) {
    if (!dailyTaskTrendChartView) return;

    QChart *chart = dailyTaskTrendChartView->chart();
    QBarSeries *series = qobject_cast<QBarSeries*>(chart->series()[0]);
    if (!series || series->barSets().isEmpty()) return;

    QBarSet *barSet = series->barSets()[0];
    barSet->remove(0, barSet->count());

    // 修改循环条件，确保只取7天
    QStringList categories;
    int maxY = 0;
    QMap<QDate, int> data = punchRecord->getPunchHistory(startDate, endDate);
    for (int i = 0; i < 7; ++i) {
        QDate date = startDate.addDays(i);
        int count = punchRecord->getPunchCount(date);
        barSet->append(count);
        categories << date.toString("MM-dd");
        maxY = std::max(maxY, count);
    }

    // 更新X轴刻度
    QBarCategoryAxis *xAxis = qobject_cast<QBarCategoryAxis*>(chart->axes(Qt::Horizontal).first());
    if (xAxis) {
        xAxis->clear();                  // 清空旧标签
        xAxis->append(categories);       // 添加新标签
    }

    // 自动调整Y轴范围
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    if (yAxis) {
        yAxis->setRange(0, std::max(5, maxY + 1));
    }
}

void DataVisualization::initPunchCalendar() {
    punchCalendar = new QCalendarWidget();
    punchCalendar->setGridVisible(true);

    // 高亮打卡日期
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::green);
    highlightFormat.setToolTip("当日有打卡记录");

    for (const QDate &date : punchRecord->getAllRecords().keys()) {
        punchCalendar->setDateTextFormat(date, highlightFormat);
    }

    connect(punchCalendar, &QCalendarWidget::clicked, this, &DataVisualization::onCalendarDateClicked);
}

void DataVisualization::onCalendarDateClicked(const QDate &date) {
    QStringList tasks = punchRecord->getTasksOfDate(date);
    if (tasks.isEmpty()) {
        QMessageBox::information(this, "当天记录",
                                 date.toString("yyyy-MM-dd") + "\n\n❌ 当天没有完成任务。");
        return;
    }

    QString msg = date.toString("yyyy-MM-dd") + "\n\n✅ 当天完成任务：\n";
    for (const auto &t : tasks)  msg += "• " + t + '\n';

    QMessageBox::information(this, "当天记录", msg.trimmed());
}

void DataVisualization::updateCharts() {
    // 保留原有图表对象，仅更新数据
    if (!completionRateChartView) {
        initCompletionRateChart();
    } else {
        // 更新饼图数据
        QPieSeries *series = qobject_cast<QPieSeries*>(completionRateChartView->chart()->series()[0]);
        series->clear();

        int completed = 0, total = taskManager->getTasks().size();
        for (const Task &task : taskManager->getTasks()) {
            if (task.isCompleted()) completed++;
        }
        series->append("已完成", completed);
        series->append("未完成", total - completed);
    }

    if (!dailyTaskTrendChartView) {
        initDailyTaskTrendChart();
    } else {
        // 更新趋势图数据
        QChart *chart = dailyTaskTrendChartView->chart();
        QBarSeries *series = qobject_cast<QBarSeries*>(chart->series()[0]);
        if (!series || series->barSets().isEmpty()) return;

        QBarSet *barSet = series->barSets()[0];
        barSet->remove(0, barSet->count());

        QDate today = QDate::currentDate();
        QDate startDate = today.addDays(-6);
        currentStartDate = startDate;
        currentEndDate = today;

        int maxY = 0;
        for (int i = 0; i < 7; ++i) {
            QDate date = startDate.addDays(i);
            int count = punchRecord->getPunchCount(date);
            barSet->append(count);  // 只追加 y 值（完成数量）
            maxY = std::max(maxY, count);
        }

        // 更新 X 轴
        QDateTimeAxis *xAxis = qobject_cast<QDateTimeAxis*>(chart->axes(Qt::Horizontal).first());
        if (xAxis) {
            xAxis->setRange(QDateTime(startDate, QTime(0, 0)), QDateTime(today, QTime(23, 59)));
        }

        // 更新 Y 轴（动态范围）
        QValueAxis *yAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
        if (yAxis) {
            yAxis->setRange(0, std::max(5, maxY + 1));
        }
    }
}

void DataVisualization::setupNavigationButtons() {
    // 创建按钮容器
    QWidget *navWidget = new QWidget();
    QHBoxLayout *navLayout = new QHBoxLayout(navWidget);
    navLayout->setContentsMargins(0, 0, 0, 0);

    // 左移按钮
    QPushButton *leftBtn = new QPushButton("◀ 前一周");
    leftBtn->setFixedWidth(80);

    // 右移按钮
    QPushButton *rightBtn = new QPushButton("后一周 ▶");
    rightBtn->setFixedWidth(80);

    // 日期范围标签
    QLabel *dateRangeLabel = new QLabel();
    dateRangeLabel->setAlignment(Qt::AlignCenter);

    // 添加到布局
    navLayout->addWidget(leftBtn);
    navLayout->addWidget(dateRangeLabel, 1);  // 居中扩展
    navLayout->addWidget(rightBtn);

    // 将导航栏添加到主布局
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(this->layout());
    mainLayout->insertWidget(0, navWidget);  // 添加到图表上方

    // 连接按钮信号
    connect(leftBtn, &QPushButton::clicked, [this]() {
        adjustDateRange(-7);  // 向前调整7天
    });

    connect(rightBtn, &QPushButton::clicked, [this]() {
        adjustDateRange(7);  // 向后调整7天
    });
}

void DataVisualization::updateDateDisplay() {
    if (!dateRangeLabel) return;

    QString format = "MM-dd";  // 更简洁的显示格式
    dateRangeLabel->setText(
        QString("%1 ~ %2")
            .arg(currentStartDate.toString(format))
            .arg(currentEndDate.toString(format))
        );

    // 禁用/启用按钮（边界控制）
    leftBtn->setEnabled(currentStartDate > QDate::currentDate().addDays(-365));
    rightBtn->setEnabled(currentEndDate < QDate::currentDate());
}

void DataVisualization::updateDateDisplay(QLabel *label) {
    if (!label) return;
    QString format = "yyyy-MM-dd";
    label->setText(QString("%1 至 %2")
                       .arg(currentStartDate.toString(format))
                       .arg(currentEndDate.toString(format)));
}

void DataVisualization::handleTaskCompleted() {
    // 强制立即更新图表
    updateTrendChartData(currentStartDate, currentEndDate);

    // 调试输出
    qDebug() << "图表已刷新，日期范围:"
             << currentStartDate.toString("yyyy-MM-dd")
             << "到"
             << currentEndDate.toString("yyyy-MM-dd");
}

void DataVisualization::updateDateRangeLabel() {
    QString rangeText = QString("%1 - %2")
    .arg(currentStartDate.toString("yyyy-MM-dd"))
        .arg(currentEndDate.toString("yyyy-MM-dd"));
    dateRangeLabel->setText(rangeText);
}
