#-------------------------------------------------
#
# Project created by QtCreator 2018-12-14T17:22:08
#
#-------------------------------------------------
include($$PWD/../Globe/vtk7_1.pri)
QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyVTKTest
TEMPLATE = app


#在.pro中引入TBB
TBB_INCLUDE=F:\vtk-m\tbb-2018_U5
TBB_LIB=F:\vtk-m\tbb-2018_U5\build\windows_ia32_gcc_mingw_release
TBB_LIBD=F:\vtk-m\tbb-2018_U5\build\windows_ia32_gcc_mingw_debug
INCLUDEPATH += $${TBB_INCLUDE}/include

LIBS += -L$${TBB_LIBD} \
        -ltbb_debug

SOURCES += main.cpp\
        mainwindow.cpp \
        ../Globe/myfunc.cpp

HEADERS  += mainwindow.h \
            MouseInteractorStyle.h \
        ../Globe/myfunc.h

FORMS    += mainwindow.ui
