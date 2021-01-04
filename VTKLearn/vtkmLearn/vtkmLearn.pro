#-------------------------------------------------
#
# Project created by QtCreator 2018-06-12T17:09:57
#
#-------------------------------------------------
include($$PWD/vtk8_1.pri)

QT       += core gui
CONFIG   += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vtkmLearn
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
#    shadertest.cpp \
    vtkmtest.cpp

HEADERS += \
    GlobeInclude.h \
#    shadertest.h \
    vtkmtest.h \
    LoadShaders.h \
    quaternion.h \
    RenderTest.h


#使用release进行编译,暂无debug运行�?
