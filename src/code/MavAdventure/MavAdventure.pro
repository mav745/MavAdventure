#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T19:40:32
#
#-------------------------------------------------

QT       += core gui opengl


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MavAdventure
TEMPLATE = app

DESTDIR = ..\..\..\build

INCLUDEPATH += .\

RC_FILE = mavadven.rc

DEFINES -= UNICODE

DEFINES += _USE_MATH_DEFINES

SOURCES += main.cpp\
        mainwindow.cpp \
    cMainManager.cpp \
    cGameRoom.cpp \
    cBaseObject.cpp \
    cTileManager.cpp \
    cBaseTexture.cpp \
    cSprite.cpp \
    util.cpp \
    cMaverick.cpp \
    cRing.cpp \
    cBaseAnimating.cpp \
    cBaseRenderable.cpp \
    cTileLayer.cpp \
    cBigDialog.cpp \
    cBaseMenu.cpp \
    cMainMenu.cpp \
    cBaseTrigger.cpp \
    cStringTrigger.cpp \
    cChangelevel.cpp \
    cFader.cpp \
    fileBuffer.cpp \ 
    cSegment.cpp \
    cLine.cpp \
    cBasePlayer.cpp \
    cConsole.cpp \
    cHud.cpp \
    cBaseTalker.cpp \
    cNpcHellen.cpp \
    triggers.cpp \
    cBaseWeapon.cpp \
    cgauss.cpp

HEADERS  += mainwindow.h \
    cMainManager.h \
    cGameRoom.h \
    cBaseObject.h \
    cTileManager.h \
    cBaseTexture.h \
    cSprite.h \
    util.h \
    cMaverick.h \
    cRing.h \
    cBaseAnimating.h \
    cBaseRenderable.h \
    cTileLayer.h \
    cBigDialog.h \
    cBaseMenu.h \
    cMainMenu.h \
    cBaseTrigger.h \
    cStringTrigger.h \
    objectTypes.h \
    cChangelevel.h \
    cFader.h \
    fileBuffer.h \
    cSegment.h \
    cLine.h \
    cBasePlayer.h \
    cConsole.h \
    cHud.h \
    cBaseTalker.h \
    cNpcHellen.h \
    triggers.h \
    cBaseWeapon.h \
    cgauss.h

FORMS    += mainwindow.ui
