#ifndef BSTU_VOLUME_POLYGON_H
#define BSTU_VOLUME_POLYGON_H

#include <vector>
#include <stack>
#include "Vertex.h"
#include "Plane.h"
#include "polygon.h"
#include "lib_geometry_global.h"

namespace bstu
{
		
class Polyhedron;

class Halver;

class Surface;

class LIB_GEOMETRYSHARED_EXPORT VolumePolygon : public Polygon
{
	// Указатели на дочерние многоугольники
	VolumePolygon* C1,* C2;
	
	// Указатель на сопряжённый многоугольник
	VolumePolygon* L;

	// Многогранник, которому принадлежит текущий многоугольник
	Polyhedron* T;

	// Поверхность, которой принадлежит многоугольник
	Surface* S;

private:

	void init(void);

	// Конструктор грани по нескольким точкам A, лежащим на плоскости plane
	VolumePolygon(const Vertex* A, int count, const Plane& plane, bool need_sort);

public:
																									
	VolumePolygon(void);

	VolumePolygon(const VolumePolygon* polygon);

	// Конструктор грани по нескольким точкам
	VolumePolygon(const std::vector<Vertex> vertices);

	// Конструктор грани по нескольким точкам
	VolumePolygon(const Vertex* A, int count);

	// Разбивает многоугольник плоскостью на два многоугольника.
	// Возвращает значение "истина", если удалось разбить многоугольник, иначе возвращает значение "ложь"
	// Добавляет в структуру halve_data информацию о разделении многоугольника
	bool halve(Halver& halve_data);

	static bool finishHalf(Halver& halve_data);

	// Возвращает значение "истина", если многоугольник разделён пополам, иначе возвращает значение "ложь"
	bool isHalve();

	// Возвращает первый дочерний многогранник
	VolumePolygon* child1() const;

	// Возвращает второй дочерний многогранник
	VolumePolygon* child2() const;

	// Возвращает соседний многогранник
	Polyhedron* neighbor() const;

	// Полностью связывает дочерние многоугольники текущего многоугольника с сопряжённым многоугольником L (и создаёт дочерние, если их ещё нет)
	void link();

	void dispose(void);

	~VolumePolygon(void);

	// Возвращает количество многоугольников, находящихся в динамической памяти (используется при отладке)
	static int alloc_count(); 

	friend class Halver;

	friend class Polyhedron;

	template <class Walker> friend void deepWalk(Polyhedron& p2, Walker &, int);				
	
	void check();

    // Итератор для обхода всех дочерних многоугольников
	class walker
	{
		VolumePolygon* current;
		std::stack<VolumePolygon*> st;
	  public:
		walker(VolumePolygon* start); // Конструктор
		operator VolumePolygon*() const;  // Перегрузка оператора разъименования
		VolumePolygon* operator*() const; // Приведение к типу VolumePolygon*
		void operator++(void); // Переход к следующему многоугольнику
	};
};

} // namespace bstu


#endif // BSTU_VOLUME_POLYGON_H
