#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T10:33:51
#
#-------------------------------------------------
include($$PWD/../Globe/vtk7_1.pri)
QT       += core gui
CONFIG  += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FourPanelViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
            interactortest.h \

FORMS    += mainwindow.ui
