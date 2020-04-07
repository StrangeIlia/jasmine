#-------------------------------------------------
#
# Project created by QtCreator 2019-12-13T18:05:53
#
#-------------------------------------------------

QT       += core gui opengl 3dcore 3drender 3dinput 3dextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jasmine
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



win32: LIBS += -L$$PWD/lib/ -llib_geometry

INCLUDEPATH += $$PWD/geometry
DEPENDPATH += $$PWD/geometry

LIBS += -lopengl32 -lglu32

DISTFILES +=

FORMS += \
    mainwindow.ui

HEADERS += \
    ModelViewerWidget.h \
    MyOrbitCameraController_Qt3D.h \
    PolyhedronTreeView.h \
    PolyhedronTreeView_Qt3D.h \
    geometry/Halver.h \
    geometry/JMath.h \
    geometry/Plane.h \
    geometry/Polygon.h \
    geometry/Polyhedron.h \
    geometry/Ray.h \
    geometry/Surface.h \
    geometry/Tree.h \
    geometry/Vector.h \
    geometry/Vertex.h \
    geometry/VolumePolygon.h \
    geometry/lib_geometry_global.h \
    mainwindow.h \
    modelviewerwidget_qt3d.h \
    ui_mainwindow.h \
    v2/CameraControllerFactory_MyOrbitCamera.h \
    v2/EntityController_Polyhedron.h \
    v2/GeometryRendererFactory_Polyhedron.h \
    v2/ICameraControllerFactory.h \
    v2/IGeometryRendererFactory.h \
    v2/PolyhedronTreeView_v2.h \
    v2/View3D.h

SOURCES += \
    ModelViewerWidget.cpp \
    MyOrbitCameraController_Qt3D.cpp \
    PolyhedronTreeView.cpp \
    PolyhedronTreeView_Qt3D.cpp \
    main.cpp \
    mainwindow.cpp \
    modelviewerwidget_qt3d.cpp \
    v2/CameraControllerFactory_MyOrbitCamera.cpp \
    v2/EntityController_Polyhedron.cpp \
    v2/GeometryRendererFactory_Polyhedron.cpp \
    v2/ICameraControllerFactory.cpp \
    v2/IGeometryRendererFactory.cpp \
    v2/PolyhedronTreeView_v2.cpp \
    v2/View3D.cpp
