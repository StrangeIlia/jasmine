#ifndef LIB_GEOMETRY_GLOBAL_H
#define LIB_GEOMETRY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIB_GEOMETRY_LIBRARY)
#  define LIB_GEOMETRYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIB_GEOMETRYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIB_GEOMETRY_GLOBAL_H
