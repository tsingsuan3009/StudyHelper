#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "settings.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(Settings *settings, QWidget *parent = nullptr);

private slots:
    void saveSettings();  // 用户点击“保存”时

private:
    Settings *settings;

    QSpinBox *reminderIntervalSpin;
    QCheckBox *reviewCheckBox;
    QComboBox *themeCombo;
};

#endif // SETTINGSDIALOG_H
