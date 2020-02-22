#ifndef BSTU_RAY_H
#define BSTU_RAY_H

#include "Vertex.h"
#include "lib_geometry_global.h"

namespace bstu
{

// Класс - луч

class LIB_GEOMETRYSHARED_EXPORT Ray
{
  public:

	Vertex A; // Начало луча

	Vector N; // Направление луча  

	Ray();

	// Конструктор луча по двум точкам. Если is_sort = "истина", то точки A и B упорядочиваются
	Ray(const Vertex& A, const Vertex& B, bool is_sort = false);

};

} // namespace bstu

#endif // BSTU_RAY_H
