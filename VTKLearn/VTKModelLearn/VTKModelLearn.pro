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
    volumeinfo.cpp \
    ggmath.cpp \
    ../Globe/myfunc.cpp \
    vecontrol.cpp \
    polydataope.cpp \
    heartseg.cpp
#    ctkwidgettest.cpp

HEADERS  += mainwindow.h \
#    ctkwidgettest.h
    interactortest.h \
    mymath.h \
    showimage.h \
    volumeinfo.h \
    ggmath.h \
    ../Globe/myfunc.h \
    MyDiocmInteractorStyleImage.h \
    MouseInteractorStyle.h \
    vecontrol.h \
    polydataope.h \
    heartseg.h

FORMS    += mainwindow.ui \
    volumeinfo.ui \
    polydataope.ui

QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp -lpthread

