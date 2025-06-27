#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include "taskmanager.h"
#include "datavisualization.h"
#include "reminder.h"
#include "punchrecord.h"
#include "settings.h"
#include "settingsdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void requestChartUpdate();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTask();
    void onTaskComplete();
    void onShowTaskDetails(const QModelIndex &index);
    void onRecommendTasks();
    void onRefreshClicked();
    void handleReminder(const QString &topic, const QString &msg);
    void applyTheme(const QString &theme);

private:
    void setupUI();
    void setupConnections();
    void refreshTaskList();
    void reloadTasks();

    // UI Components
    QTabWidget *tabWidget;
    QWidget *taskTab;
    QTableWidget *taskTable;
    QPushButton *addTaskButton;
    QPushButton *completeButton;
    QPushButton *refreshButton;
    QPushButton *settingsButton;
    QPushButton *recommendBtn;
    SettingsDialog *settingsDialog;
    Settings *settings;

    // Functional Modules
    TaskManager *taskManager;
    PunchRecord *punchRecord;
    Reminder *reminder;
    DataVisualization *dataViz;
};

#endif // MAINWINDOW_H
