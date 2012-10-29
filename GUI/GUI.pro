#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T18:50:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUI
TEMPLATE = app


SOURCES += \
    main.cpp \
    gui.cpp \
    process_threads.cpp \
    parameterdialog.cpp

HEADERS  += gui.h \
    process_threads.h \
    parameterdialog.h

FORMS    +=

INCLUDEPATH += $$PWD/../Engine
DEPENDPATH += $$PWD/../Engine

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Engine/release/ -lEngine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Engine/debug/ -lEngine
else:unix: LIBS += -L$$OUT_PWD/../Engine/ -lEngine

RESOURCES     = gui.qrc

