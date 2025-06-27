QT += core gui widgets charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analysis.cpp \
    callout.cpp \
    datavisualization.cpp \
    main.cpp \
    mainwindow.cpp \
    punchrecord.cpp \
    reminder.cpp \
    reviewreminder.cpp \
    settings.cpp \
    settingsdialog.cpp \
    systemrecommendation.cpp \
    task.cpp \
    taskmanager.cpp

HEADERS += \
    analysis.h \
    callout.h \
    datavisualization.h \
    mainwindow.h \
    punchrecord.h \
    reminder.h \
    reviewreminder.h \
    settings.h \
    settingsdialog.h \
    systemrecommendation.h \
    task.h \
    taskmanager.h

# 构建目录设置
build_target: DESTDIR = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR = $$OUT_PWD/moc
UI_DIR = $$OUT_PWD/ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += $$OUT_PWD

RESOURCES += \
    resources.qrc
