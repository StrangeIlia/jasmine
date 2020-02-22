#ifndef BSTU_POLYGON_H
#define BSTU_POLYGON_H

#include <vector>
#include "Vertex.h"
#include "Plane.h"
#include "lib_geometry_global.h"

namespace bstu
{


class LIB_GEOMETRYSHARED_EXPORT Polygon
{

protected:

	// Массив вершин многоугольника
	std::vector<Vertex> V;

    // Плоскость, на которой лежит многоугольник
    Plane _plane;

	// Площадь многоугольника
    double _mes;

	// Конструктор грани по нескольким точкам A, лежащим на плоскости plane
	// count - количество точек в массиве A
	// Если need_sort = "истина", то вершины A будут упорядочиваться по часовой стрелке
	Polygon(const Vertex* A, int count, const Plane& plane, bool need_sort);

public:

	Polygon(void);

	// Конструктор грани по нескольким точкам
	Polygon(const std::vector<Vertex> vertices);

	// Конструктор грани по нескольким точкам
	Polygon(const Vertex* A, int count);

	bool operator == (const Polygon& P) const;

	bool operator != (const Polygon& P) const;

	// Возвращает значение "истина", если луч ray пересекает многоугольник, иначе возвращает значение "ложь" 
    bool intersects(const Ray& ray, Vertex& R) const;

	// Возвращает значение "истина", если отрезок AB пересекается с многоугольником, иначе возвращает значение "ложь" 
    bool intersects(const Vertex& A, const Vertex& B, Vertex& R) const;

	// Возвращает истину, если стороны текущего многоугольника пересекаются со вторым многоугольником 
	bool intersects(const Polygon& polygon) const;

	// Разбивает многоугольник плоскостью plane на два многоугольника
	// plane - уравнение секущей плоскости
	// A - массив вершин первого многоугольника, упорядоченный по часовой стрелке
	// B - массив вершин второго многоугольника (тоже упорядоченный)
	// D - массив общих вершин двух многоугольников A и B
	// count[0], count[1], count[2] - количество записанных вершин, соответственно, в массивы A, B, D
	int halve(const Plane& plane, Vertex* A, Vertex* B, Vertex* D, int* count) const;	

	// Возвращает уравнение плоскости, на которой лежит многоугольник
	Plane plane() const;

	// Возвращает площадь многоугольника
	double mes() const;

	// Возвращает координаты центра многоугольника
	Vertex center() const;

	// Возвращает координаты вершины многоугольника с заданным индексом index
	Vertex vertex(int index) const;

	// Возвращает количество вершин многоугольника
	unsigned int size() const;

	// Возвращает значение "истина", если вершины многоугольника V упорядочены по часовой стрелке относительно вектора нормали (plane.A, plane.B, plane.C), или "ложь" в противном случае
	// Данная функция используется для рисования многоугольника в OpenGL
	bool isClockwise(void) const;

	// Выводит в консоль список вершин многоугольника
	void print(void);

	~Polygon(void);

protected:


private:

	// Функция возвращает площадь многоугольника 
	static double get_mes(const Vertex* V, unsigned int size);

	// Подпрограмма сортирует вершины по часовой стрелке
	void sort_vertices();

	void init(void);

	// Функция возвращает значение "истина", если точка A находится внутри многоугольника, иначе "ложь"
	bool inside(const Vertex& A) const;

	// Удаляет одинаковые вершины в отсортированном массиве вершин
	void deleteEquals();
};


} // namespace bstu


#endif // BSTU_POLYGON_H
