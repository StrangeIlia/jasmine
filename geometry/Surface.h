#ifndef BSTU_SURFACE_H
#define BSTU_SURFACE_H

#include "Tree.h"
#include "VolumePolygon.h"
#include "lib_geometry_global.h"

namespace bstu
{

class Tree;

class LIB_GEOMETRYSHARED_EXPORT Surface
{

public:

	std::vector<Polygon*> edges;

	std::vector<VolumePolygon*> polygons;

	Surface(void);

	~Surface(void);

	static Surface* createBox(const Vertex& A, const Vertex& B, Tree* tree);

};



} // namespace bstu


#endif // BSTU_SURFACE_H
