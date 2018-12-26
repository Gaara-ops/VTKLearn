#-------------------------------------------------
#
# Project created by QtCreator 2018-05-29T15:12:12
#
#-------------------------------------------------
include($$PWD/../Globe/vtk7_1.pri)
#include($$PWD/ctk.pri)

QT       += core gui
CONFIG      += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VTKModelLearn
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    showimage.cpp \
    myraycastimage.cpp \
    volumeinfo.cpp \
    ggmath.cpp \
    myfunc.cpp
#    ctkwidgettest.cpp

HEADERS  += mainwindow.h \
#    ctkwidgettest.h
    interactortest.h \
    mymath.h \
    showimage.h \
    myraycastimage.h \
    volumeinfo.h \
    ggmath.h \
    myfunc.h \
    MyDiocmInteractorStyleImage.h \
    MouseInteractorStyle.h

FORMS    += mainwindow.ui \
    volumeinfo.ui
