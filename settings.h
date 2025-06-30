#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject *parent = nullptr);

    // 提醒相关设置
    int getReminderInterval() const;      // 提醒频率（分钟）
    void setReminderInterval(int minutes);

    bool isReviewEnabled() const;         // 是否开启复习提醒
    void setReviewEnabled(bool enabled);

    // 主题设置
    QString getThemePreference() const;   // 主题：light/dark/system
    void setThemePreference(const QString &theme);

    // 加载和保存
    void loadSettings();
    void saveSettings();

signals:
    void settingsChanged();  // 可用于界面响应更新

private:
    int reminderInterval;     // 提醒间隔（分钟）
    bool reviewEnabled;       // 是否启用复习提醒
    QString themePreference;  // 主题：light/dark/system
    QSettings qsettings;

};

#endif // SETTINGS_H
