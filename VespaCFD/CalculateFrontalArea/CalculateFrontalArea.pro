#-------------------------------------------------
#
# Project created by QtCreator 2012-06-23T11:47:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CalculateFrontalArea
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# You will need to change this to suit how you have VTK installed on your system
# VTK_INCLUDE_DIR is a system variable setting on my Centos and VTK_DIR on my Windows

INCLUDEPATH += $(VTK_DIR)\\Common \
               $(VTK_DIR)\\IO \
               $(VTK_DIR)\\Filtering \
               $(VTK_DIR)\\Rendering \
               $(VTK_DIR)\\Graphics \
               $(VTK_DIR)\\build \
               $(VTK_INCLUDE_DIR) 

LIBS +=     -L$(VTK_DIR)\\Build\\bin\\Debug \
            -L$(VTK_LIB_DIR) \
            -lvtkCommon -lvtkRendering -lvtkGraphics -lvtkFiltering -lvtkIO -lvtksys

SOURCES += CalculateFrontalArea.cxx
