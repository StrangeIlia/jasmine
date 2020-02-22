#ifndef BSTU_HALVEDATA_H
#define BSTU_HALVEDATA_H

#include "Polyhedron.h"
#include <stdio.h>
#include <math.h>
#include "lib_geometry_global.h"

namespace bstu
{

class VolumePolygon;
class Polyhedron;

class LIB_GEOMETRYSHARED_EXPORT Halver
{
	// Секущая плоскость
	const Plane _plane;

    Vertex V[128]; 
	
	// P1[0] и P2[0] - многоугольники, полученные в результате рассечения плоскостью plane.
	// Остальные получены в результате сортировки относительно секущей плоскости.
	// P1[i] (i=1, n1-1) расположены выше секущей плоскости, P2[i] (i=1, n2-1) - ниже секущей плоскости.
    VolumePolygon * P1[128], * P2[128];

    int n1, n2, n;

public:

	Polyhedron* T1, * T2;

    Halver(const Plane& plane);

	Plane plane() const;

    // Процедура проверяет, есть ли такая вершина в массиве V и если нет, то добавляет её в этот массив
    void add(const Vertex& vertex);

    // Добавить многогранник, лежащий выше плоскости
    void addUpper(VolumePolygon* p);

    // Добавить многогранник, лежащий ниже плоскости
    void addLower(VolumePolygon* p);

	void reset(void);

	// Функция создаёт два многоугольника P[0] и P[1], собранных из вершин V
	// Возвращает значение "истина", если многоугольники успешно созданы, т.е. количество собранных вершин больше двух, иначе возвращает значение "ложь"
	bool finishHalf(void);

	// Процедура определяет действия, которые нужно совершить над созданными многоугольниками
	virtual void withChild(VolumePolygon* child);

	// Определяет, необходимо ли делить многогранник?
	virtual bool needHalve(Polyhedron* P) const;
};


} // namespace bstu


#endif // BSTU_HALVEDATA_H
