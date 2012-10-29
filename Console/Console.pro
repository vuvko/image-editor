#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T18:53:01
#
#-------------------------------------------------

QT       += core

QT       += gui

TARGET = Console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    arg_parser.cpp

HEADERS += \
    arg_parser.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Engine/release/ -lEngine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Engine/debug/ -lEngine
else:unix:!macx:!symbian: LIBS += -L$$OUT_PWD/../Engine/ -lEngine

INCLUDEPATH += $$PWD/../Engine
DEPENDPATH += $$PWD/../Engine
