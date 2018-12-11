###
# Author:  Tomas Aubrecht (xaubre02)
# Date:    may 2018
# Desc:    Project build file
###

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blockeditor
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

SOURCES   += \
          main.cpp \
          core/block.cpp \
          core/myblocks.cpp \
          gui/connector.cpp \
          gui/editoritem.cpp \
          gui/editornote.cpp \
          gui/editorscene.cpp \
          gui/mainwindow.cpp \
          gui/savedata.cpp

HEADERS   += \
          core/block.h \
          core/myblocks.h \
          gui/connector.h \
          gui/editoritem.h \
          gui/editornote.h \
          gui/editorscene.h \
          gui/mainwindow.h \
          gui/savedata.h 

RESOURCES += \
          resources.qrc

RC_FILE   = appicon.rc
