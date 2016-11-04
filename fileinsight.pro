#-------------------------------------------------
#
# Project created by QtCreator 2016-09-13T13:07:54
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = fileinsight
TEMPLATE = app


SOURCES += main.cpp\
        fileinsight.cpp \
    fileinsightsubdialog.cpp

HEADERS  += fileinsight.h \
    fileinsightsubdialog.h \
    constants.h

FORMS    += fileinsight.ui \
    fileinsightsubdialog.ui

# When compiling on Windows, load our thirdparty/ folder
win32:INCLUDEPATH += $$PWD\thirdparty\include
win32:LIBPATH += $$PWD\thirdparty\bin

# Link to libmagic for file type detection
LIBS += -lmagic
