#ifndef BSTU_TREE_H
#define BSTU_TREE_H

#include <vector>
#include "Polyhedron.h"
#include "Surface.h"
#include "lib_geometry_global.h"

namespace bstu
{


class LIB_GEOMETRYSHARED_EXPORT Tree
{
	std::vector<Polyhedron*> P;

public:


	Surface* surface;
	
	Tree(void);

	void addPolyhedron(Polyhedron* A);
	
	Polyhedron* polyhedron(int k);

	unsigned int polyhedronsCount(void);

	void restructure(void);

	void resetVisit(void);

	void addBox(const Vertex& A, const Vertex& B);

	void halve(Halver& halver);

	void dispose(void);

	~Tree(void);

private:

	// Процедура удаляет из динамической памяти все дочерние многоугольники многоугольника polygon, кроме листовых 
	static void dispose_except_lowers(VolumePolygon* polygon);


	friend class Surface;
};


} // namespace bstu


#endif // BSTU_TREE_H
