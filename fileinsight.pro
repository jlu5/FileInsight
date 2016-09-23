#-------------------------------------------------
#
# Project created by QtCreator 2016-09-13T13:07:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fileinsight
TEMPLATE = app


SOURCES += main.cpp\
        fileinsight.cpp \
    icondisplaywidget.cpp

HEADERS  += fileinsight.h \
    icondisplaywidget.h

FORMS    += fileinsight.ui

# Link to libmagic for file type detection
LIBS += -lmagic
