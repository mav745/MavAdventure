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
    util.cpp \
    cbasenode.cpp \
    cstriplayer.cpp \
    cframegrid.cpp \
    cframebar.cpp \
    cpreviewarea.cpp

HEADERS  += mainwindow.h \
    util.h \
    cbasenode.h \
    cstriplayer.h \
    cframegrid.h \
    cframebar.h \
    cpreviewarea.h

FORMS    += mainwindow.ui
