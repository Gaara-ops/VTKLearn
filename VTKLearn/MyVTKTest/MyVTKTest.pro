#-------------------------------------------------
#
# Project created by QtCreator 2018-12-14T17:22:08
#
#-------------------------------------------------
include($$PWD/../Globe/vtk7_1.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyVTKTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui