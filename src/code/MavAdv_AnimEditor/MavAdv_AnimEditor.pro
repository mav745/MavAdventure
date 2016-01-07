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
    util.cpp \
    canimframebar.cpp \
    cbasenode.cpp \
    cstriplayer.cpp \
    cnodearea.cpp \
    caframeswidget.cpp \
    cstripframewidget.cpp

HEADERS  += mainwindow.h \
    cpreviewframe.h \
    util.h \
    canimframebar.h \
    cbasenode.h \
    cstriplayer.h \
    cnodearea.h \
    caframeswidget.h \
    cstripframewidget.h

FORMS    += mainwindow.ui
