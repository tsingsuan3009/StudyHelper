#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QDateTime>
#include "systemrecommendation.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    taskManager(new TaskManager(this)),
    punchRecord(new PunchRecord(this)),
    reminder(new Reminder(this)),
    dataViz(new DataVisualization(taskManager, punchRecord, this)),
    settings(new Settings(this))
{
    setupUI();
    setupConnections();
    refreshTaskList();

    // Window Settings
    setWindowTitle("学习养成计划");
    resize(1000, 700);

    applyTheme(settings->getThemePreference());

    setWindowIcon(QIcon(":/icons/themes/icon.png"));
}

void MainWindow::setupUI()
{
    // Central Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Tab Widget
    tabWidget = new QTabWidget(centralWidget);

    /**************** Task Tab ****************/
    taskTab = new QWidget(tabWidget);
    QVBoxLayout *taskLayout = new QVBoxLayout(taskTab);

    // Task Table
    taskTable = new QTableWidget(taskTab);
    taskTable->setColumnCount(4);
    taskTable->setHorizontalHeaderLabels({"任务主题", "优先级", "截止时间", "状态"});
    taskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Button Panel
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addTaskButton = new QPushButton("添加任务", taskTab);
    completeButton = new QPushButton("标记完成", taskTab);
    refreshButton = new QPushButton("刷新列表", taskTab);
    settingsButton = new QPushButton("设置", taskTab);
    recommendBtn = new QPushButton("推荐任务");

    buttonLayout->addWidget(recommendBtn);
    buttonLayout->addWidget(addTaskButton);
    buttonLayout->addWidget(completeButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(settingsButton);

    // Assemble Task Tab
    taskLayout->addWidget(taskTable);
    taskLayout->addLayout(buttonLayout);
    tabWidget->addTab(taskTab, "任务管理");

    /**************** Stats Tab ****************/
    tabWidget->addTab(dataViz, "学习统计");

    // Add to Main Layout
    mainLayout->addWidget(tabWidget);
}

void MainWindow::setupConnections()
{
    // Buttons
    connect(addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(completeButton, &QPushButton::clicked, this, &MainWindow::onTaskComplete);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(settingsButton, &QPushButton::clicked, this, [=]() {settingsDialog = new SettingsDialog(settings, this);settingsDialog->exec();});
    connect(recommendBtn, &QPushButton::clicked, this, &MainWindow::onRecommendTasks);

    // Table
    connect(taskTable, &QTableWidget::doubleClicked, this, &MainWindow::onShowTaskDetails);

    // Reminder System
    connect(reminder, &Reminder::reminderTriggered, this, &MainWindow::handleReminder);

    // Data Updates
    connect(taskManager, &TaskManager::tasksChanged, this, &MainWindow::refreshTaskList);
    connect(taskManager, &TaskManager::tasksChanged, dataViz, &DataVisualization::updateCharts);

    connect(taskManager, &TaskManager::taskAdded, dataViz, &DataVisualization::updateCharts);
    connect(taskManager, &TaskManager::tasksChanged, dataViz, &DataVisualization::updateCharts);
    connect(taskManager, &TaskManager::taskCompleted,dataViz, &DataVisualization::handleTaskCompleted);
    connect(punchRecord, &PunchRecord::punchRecorded, dataViz, &DataVisualization::updateCharts);
    connect(punchRecord, SIGNAL(punchRecorded(QString,QDate)),dataViz, SLOT(updateTrendChartData()));
    connect(settings, &Settings::settingsChanged, this, [=]() {applyTheme(settings->getThemePreference());});
}

void MainWindow::onRefreshClicked() {
    taskManager->reloadTasks();         // 重新读取数据库
}

void MainWindow::refreshTaskList()
{
    taskTable->setRowCount(0);
    const auto &tasks = taskManager->getTasks();

    for (int i = 0; i < tasks.size(); ++i) {
        const Task &task = tasks[i];
        taskTable->insertRow(i);

        // Task Topic
        taskTable->setItem(i, 0, new QTableWidgetItem(task.getTopic()));

        // Priority
        QString priority;
        switch (task.getPriority()) {
        case Task::High: priority = "高"; break;
        case Task::Medium: priority = "中"; break;
        case Task::Low: priority = "低"; break;
        }
        taskTable->setItem(i, 1, new QTableWidgetItem(priority));

        // Due Time
        taskTable->setItem(i, 2,
                           new QTableWidgetItem(task.getDueTime().toString("yyyy-MM-dd hh:mm")));

        // Status
        taskTable->setItem(i, 3,
                           new QTableWidgetItem(task.isCompleted() ? "✅ 完成" : "🕒 进行中"));
    }
}

void MainWindow::onAddTask() {
    // 创建自定义对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加新任务");

    // 表单布局
    QFormLayout form(&dialog);

    // 任务名称输入框
    QLineEdit nameEdit;
    form.addRow("任务名称:", &nameEdit);

    // 截止时间选择器
    QDateTimeEdit dateTimeEdit;
    dateTimeEdit.setDateTime(QDateTime::currentDateTime().addDays(1)); // 默认明天此时
    dateTimeEdit.setDisplayFormat("yyyy-MM-dd HH:mm");
    form.addRow("截止时间:", &dateTimeEdit);

    // 优先级选择
    QComboBox priorityCombo;
    priorityCombo.addItems({"高", "中", "低"});
    form.addRow("优先级:", &priorityCombo);

    // 需要复习复选框
    QCheckBox reviewCheck("需要复习提醒");
    reviewCheck.setChecked(true);
    form.addRow(&reviewCheck);

    // 按钮框
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    // 连接按钮信号
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        // 获取输入值
        QString topic = nameEdit.text().trimmed();
        if (topic.isEmpty()) {
            QMessageBox::warning(this, "错误", "任务名称不能为空");
            return;
        }

        // 转换优先级
        Task::Priority priority = Task::Medium;
        switch (priorityCombo.currentIndex()) {
        case 0: priority = Task::High; break;
        case 1: priority = Task::Medium; break;
        case 2: priority = Task::Low; break;
        }

        // 创建任务
        Task task(topic,
                  dateTimeEdit.dateTime(),
                  priority,
                  reviewCheck.isChecked());

        // 添加到系统
        taskManager->addTask(task);
        reminder->addTask(task);
    }
}

void MainWindow::onTaskComplete() {
    int row = taskTable->currentRow();
    if (row >= 0 && row < taskManager->getTasks().size()) {
        QString taskTopic = taskManager->getTasks()[row].getTopic();

        // 标记任务完成
        taskManager->markTaskCompleted(row, true);

        // 记录打卡
        punchRecord->addPunchDetail(taskTopic);

        // 更新任务列表显示
        refreshTaskList();

        // 强制刷新图表
        dataViz->updateCharts();  // 确保刷新图表
    }
}

void MainWindow::onShowTaskDetails(const QModelIndex &index)
{
    int row = index.row();
    if (row >= 0 && row < taskManager->getTasks().size()) {
        const Task &task = taskManager->getTasks()[row];
        QString details = QString(
                              "📝 任务详情\n\n"
                              "主题: %1\n"
                              "优先级: %2\n"
                              "截止时间: %3\n"
                              "状态: %4\n"
                              "需要复习: %5")
                              .arg(task.getTopic())
                              .arg(task.getPriority() == Task::High ? "高" :
                                       task.getPriority() == Task::Medium ? "中" : "低")
                              .arg(task.getDueTime().toString("yyyy-MM-dd hh:mm"))
                              .arg(task.isCompleted() ? "已完成" : "进行中")
                              .arg(task.needsReview() ? "是" : "否");

        QMessageBox::information(this, "任务详情", details);
    }
}

void MainWindow::handleReminder(const QString &topic, const QString &msg)
{
    QMessageBox::warning(this, "任务提醒", msg);

    // Highlight corresponding task
    for (int i = 0; i < taskTable->rowCount(); ++i) {
        if (taskTable->item(i, 0)->text() == topic) {
            taskTable->selectRow(i);
            break;
        }
    }
}

void MainWindow::applyTheme(const QString &theme) {
    qApp->setStyleSheet("");  // 先清除样式

    if (theme == "dark") {
        QFile file(":/themes/dark.qss");
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            qApp->setStyleSheet(file.readAll());
        }
    } else if (theme == "light") {
        QFile file(":/themes/light.qss");
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            qApp->setStyleSheet(file.readAll());
        }
    }
}


MainWindow::~MainWindow()
{
    // All Qt objects are child of MainWindow and will be deleted automatically
}

void MainWindow::onRecommendTasks() {
    SystemRecommendation recommender(punchRecord);
    QList<Task> tasks = recommender.generateRecommendations();

    for (const Task &t : tasks) {
        taskManager->addTask(t);
    }

    refreshTaskList();
    QMessageBox::information(this, "推荐完成", QString("已为你推荐 %1 个任务。").arg(tasks.size()));
}
