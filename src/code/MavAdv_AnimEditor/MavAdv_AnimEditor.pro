#-------------------------------------------------
#
# Project created by QtCreator 2016-01-03T17:25:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MavAdv_AnimEditor
TEMPLATE = app

DESTDIR = ../../../build


SOURCES += main.cpp\
        mainwindow.cpp \
    cpreviewframe.cpp \
    cstripframesframe.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    cpreviewframe.h \
    cstripframesframe.h \
    util.h

FORMS    += mainwindow.ui
