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
# VTK_INCLUDE_DIR + VTK_LIB_DIR is a system variable setting on my Centos and VTK_DIR on my Windows

win32:INCLUDEPATH += $(VTK_DIR)\\Common \
               $(VTK_DIR)\\IO \
               $(VTK_DIR)\\Filtering \
               $(VTK_DIR)\\Rendering \
               $(VTK_DIR)\\Graphics \
               $(VTK_DIR)\\build \
               $(VTK_DIR)\\Imaging \
               $(VTK_DIR)\\Hybrid

win32:LIBS += -L$(VTK_DIR)\\Build\\bin\\Debug \
              -lvtkCommon -lvtkRendering -lvtkGraphics -lvtkFiltering -lvtkIO -lvtksys -lvtkHybrid

unix:INCLUDEPATH += $(VTK_INCLUDE_DIR)

unix:LIBS += -L$(VTK_LIB_DIR) \
              -lvtkCommon -lvtkRendering -lvtkGraphics -lvtkFiltering -lvtkIO -lvtksys -lvtkHybrid

SOURCES += CalculateFrontalArea.cxx
