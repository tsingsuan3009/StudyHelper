#include "settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent), qsettings("MyCompany", "StudyPlanner") {
    loadSettings();
}

void Settings::loadSettings() {
    reminderInterval = qsettings.value("reminderInterval", 60).toInt(); // 默认60分钟
    reviewEnabled = qsettings.value("reviewEnabled", true).toBool();    // 默认启用复习提醒
    themePreference = qsettings.value("themePreference", "system").toString();
}

void Settings::saveSettings() {
    qsettings.setValue("reminderInterval", reminderInterval);
    qsettings.setValue("reviewEnabled", reviewEnabled);
    qsettings.setValue("themePreference", themePreference);
}

int Settings::getReminderInterval() const {
    return reminderInterval;
}
void Settings::setReminderInterval(int minutes) {
    reminderInterval = minutes;
    saveSettings();
    emit settingsChanged();
}

bool Settings::isReviewEnabled() const {
    return reviewEnabled;
}
void Settings::setReviewEnabled(bool enabled) {
    reviewEnabled = enabled;
    saveSettings();
    emit settingsChanged();
}

QString Settings::getThemePreference() const {
    return themePreference;
}
void Settings::setThemePreference(const QString &theme) {
    themePreference = theme;
    saveSettings();
    emit settingsChanged();
}
