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

DISTFILES += \
    .gitignore

FORMS += \
    mainwindow.ui

HEADERS += \
    EntitiesContainer.h \
    MyOrbitCameraController.h \
    PolyhedronTreeView.h \
    View3D.h \
    collections/AbstractEntityMap.hpp \
    collections/AbstractEntitySet.h \
    collections/AbstractPolyhedronSet.h \
    collections/AbstractSet.hpp \
    collections/AbstractTransformSet.h \
    collections/Enumerable.hpp \
    collections/TransformsSet.h \
    collections/enumerator/ConditionalEnumerator.hpp \
    collections/enumerator/Enumerator.hpp \
    exceptions/NotImplementedException.hpp \
    extensions/PolyhedronExtension.h \
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
    initializers/AbstractViewInitializer.hpp \
    initializers/CameraAndLightInitalizer.h \
    initializers/SelectorInitializer.h \
    mainwindow.h \
    utils/AbstractLockable.h \
    utils/Adapter.h \
    utils/Centering.hpp \
    utils/Cleaner.hpp \
    utils/EntityConstructor.h \
    utils/factories/AbstractGeometryFactory.hpp \
    utils/factories/AbstractMaterialFactory.hpp \
    utils/factories/AbstractTransformFactory.hpp \
    utils/factories/imp/SimpleGeometryFactory.h \
    utils/factories/imp/SimpleMaterialFactory.h \
    utils/factories/imp/SimpleTransformFactory.h

SOURCES += \
    EntitiesContainer.cpp \
    MyOrbitCameraController.cpp \
    PolyhedronTreeView.cpp \
    View3D.cpp \
    collections/AbstractEntityMap.cpp \
    collections/AbstractEntitySet.cpp \
    collections/AbstractPolyhedronSet.cpp \
    collections/AbstractSet.cpp \
    collections/AbstractTransformSet.cpp \
    collections/TransformsSet.cpp \
    extensions/PolyhedronExtension.cpp \
    initializers/CameraAndLightInitalizer.cpp \
    initializers/SelectorInitializer.cpp \
    main.cpp \
    mainwindow.cpp \
    utils/Adapter.cpp \
    utils/Centering.cpp \
    utils/EntityConstructor.cpp \
    utils/factories/imp/SimpleGeometryFactory.cpp \
    utils/factories/imp/SimpleMaterialFactory.cpp \
    utils/factories/imp/SimpleTransformFactory.cpp
