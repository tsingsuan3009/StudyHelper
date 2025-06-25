#include "settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), settings(settings) {
    setWindowTitle("应用设置");

    // 控件初始化
    reminderIntervalSpin = new QSpinBox(this);
    reminderIntervalSpin->setRange(5, 360);
    reminderIntervalSpin->setSuffix(" 分钟");
    reminderIntervalSpin->setValue(settings->getReminderInterval());

    reviewCheckBox = new QCheckBox("开启复习提醒", this);
    reviewCheckBox->setChecked(settings->isReviewEnabled());

    themeCombo = new QComboBox(this);
    themeCombo->addItems({"系统默认", "浅色模式", "深色模式"});
    QString theme = settings->getThemePreference();
    int index = themeCombo->findText(theme == "dark" ? "深色模式" :
                                         theme == "light" ? "浅色模式" : "系统默认");
    themeCombo->setCurrentIndex(index);

    // 布局
    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->addRow("提醒频率：", reminderIntervalSpin);
    formLayout->addRow("", reviewCheckBox);
    formLayout->addRow("主题偏好：", themeCombo);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
}

void SettingsDialog::saveSettings() {
    settings->setReminderInterval(reminderIntervalSpin->value());
    settings->setReviewEnabled(reviewCheckBox->isChecked());

    QString theme;
    switch (themeCombo->currentIndex()) {
    case 0: theme = "system"; break;
    case 1: theme = "light"; break;
    case 2: theme = "dark"; break;
    }
    settings->setThemePreference(theme);

    accept();  // 关闭对话框
}
