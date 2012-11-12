#-------------------------------------------------
#
# Project created by QtCreator 2012-06-23T11:47:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = AssembleVespa
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $(VTK_DIR)\\Common \
               $(VTK_DIR)\\IO \
               $(VTK_DIR)\\Filtering \
               $(VTK_DIR)\\Rendering \
               $(VTK_DIR)\\Graphics \
               $(VTK_DIR)\\build

LIBS +=     -LD:\\vtk-5.8.0\\build\\bin\\Debug \
            -lvtkCommon -lvtkRendering -lvtkGraphics -lvtkFiltering -lvtkIO# -lvtksys -lvtkViews -lvtkWidgets -lvtkInfovis   \
#            -lvtkImaging  -lvtklibxml2 -lvtkDICOMParser -lvtkpng -lvtkpng -lvtktiff -lvtkzlib \
#            -lvtkjpeg -lvtkalglib -lvtkexpat -lvtkverdict -lvtkmetaio -lvtkNetCDF -lvtksqlite -lvtkexoIIc -lvtkftgl \
#            -lvtkfreetype -lvtkHybrid

SOURCES += AssembleVespa.cxx
