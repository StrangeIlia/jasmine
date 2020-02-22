#ifndef BSTU_POLYHEDRON_H
#define BSTU_POLYHEDRON_H

#include <vector>
#include <stack>
#include "VolumePolygon.h"
#include "lib_geometry_global.h"

namespace bstu
{	

class VolumePolygon;

class Walker;

class LIB_GEOMETRYSHARED_EXPORT Polyhedron
{
        // Идентификатор многогранника
        int ID;

        std::vector<VolumePolygon*> polygons;

        // Дочерние многогранники
        Polyhedron* C1;

        Polyhedron* C2;

        // Объём многогранника
        double _mes;

        // Идентификатор обходчика
        int visit_id;

        Vertex _center;

private:

        double get_mes() const;

        Vertex get_center() const;

        // Инициализирует вектора нормалей у всех граней
        void right_normals(void);

public:
	
	Polyhedron(void);

	// Конструктор многогранника по нескольким точкам
	Polyhedron(VolumePolygon** P, int count);

	// Возвращает указатель на многоугольник с заданным номером index
	VolumePolygon* polygon(int index) const;

	// Возвращает количество многоугольников в многограннике
	int size() const;

	// Возвращает объём многогранника
	double volume() const;

	// Возвращает координаты центра многогранника
	Vertex center() const;

	// Возвращает значение "истина", если многоугольник polygon пересекается с многогранником или находится внутри него
	bool intersects(const Polygon& polygon) const;

	// Возвращает значение "истина", если многогранник пересекается с лучом, иначе возвращает значение "ложь"
	bool intersects(const Ray& ray, Vertex& R) const;

	// Функция возвращает значение "истина", если точка находится внутри многогранника, иначе возвращает значение "ложь"
	bool inside(const Vertex& vertex) const;

	bool halve(const Plane& halver);

	bool halve(Halver& halver);
	
	// Возвращает значение "истина", если многогранник разделён, т.е. содержит дочерние многогранники, иначе возвращает значение "ложь"
	bool isHalve() const;

	// Настраивает связи с соседними многогранниками
	void link(void);

	// Возвращает указатель на первый дочерний многогранник
	Polyhedron* child1() const;

	// Возвращает указатель на второй дочерний многогранник
	Polyhedron* child2() const;

	// Сбрасывает идентификатор обхода (visit_id) у всех многогранников, в том числе дочерних
	void resetVisit(void);

	void dispose_childs(void);

	// Функция собрирает в массив все многоугольники-листья текущего многогранника
	int collectLowers(VolumePolygon** P);

	// Шаблонная подпрограмма для рекурсивного обхода дерева многогранников
	template<class A> void forEach(A & action);

	~Polyhedron(void);

	// Возвращает количество многогранников, созданных в динамической памяти (используется при отладке)
	static int alloc_count();

	double check_mes();

	// Итератор для обхода всех дочерних многогранников
	class full_walker
	{
		Polyhedron* P;
		std::stack<Polyhedron*> st;
	  public:
		full_walker(Polyhedron* start); // Конструктор
		operator Polyhedron*() const;  // Перегрузка оператора разъименования
		Polyhedron* operator*() const; // Приведение к типу Polyhedron*
		void operator++(void); // Переход к следующему многограннику
	};

	// Итератор для обхода только многогранников-листьев
	class low_walker
	{
		Polyhedron* P;
		std::stack<Polyhedron*> st;
	  public:
		low_walker(Polyhedron* start); // Конструктор
		operator Polyhedron*() const;  // Перегрузка оператора разъименования
		Polyhedron* operator*() const; // Приведение к типу Polyhedron*
		void operator++(void); // Переход к следующему многограннику
	};

	template <class Walker> friend void deepWalk(Polyhedron& p2, Walker &, int);

};

} // namespace bstu

#endif // BSTU_POLYHEDRON_H
