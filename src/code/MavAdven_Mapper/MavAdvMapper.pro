#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T19:03:36
#
#-------------------------------------------------

QT       += core gui opengl

INCLUDEPATH += "./"

DEFINES += MAPPER_BUILD _USE_MATH_DEFINES

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MavAdvMapper
TEMPLATE = app

DESTDIR = ../../../build

SOURCES += main.cpp\
        mainwindow.cpp \
    cGLManager.cpp \
    cBase.cpp \
    cSegment.cpp \
    cLine.cpp \
    cBaseExpanded.cpp \
    cParExpand.cpp \
    cAxialExpand.cpp \
    cBaseTool.cpp \
    cLineTool.cpp \
    cBaseToolButton.cpp \
    toolbuttons.cpp \
    cTileTool.cpp \
    cTileLayer.cpp \
    cBacksTool.cpp \
    cBackground.cpp \
    triggers.cpp \
    cTriggsTool.cpp \
    cdlgentprops.cpp \
    cBaseEntityTool.cpp \
    cEntityTool.cpp \
    cBaseEntity.cpp

HEADERS  += mainwindow.h \
    cGLManager.h \
    keys.h \
    cBase.h \
    cSegment.h \
    cLine.h \
    cBaseExpanded.h \
    cParExpand.h \
    cAxialExpand.h \
    cBaseTool.h \
    cLineTool.h \
    cBaseToolButton.h \
    toolbuttons.h \
    objecttypes.h \
    cTileTool.h \
    cTileLayer.h \
    cBacksTool.h \
    cBackground.h \
    triggers.h \
    cTriggsTool.h \
    cdlgentprops.h \
    cBaseEntityTool.h \
    cEntityTool.h \
    cBaseEntity.h

FORMS    += mainwindow.ui \
    cdlgentprops.ui
