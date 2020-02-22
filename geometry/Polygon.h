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

	// ������ ������ ��������������
	std::vector<Vertex> V;

    // ���������, �� ������� ����� �������������
    Plane _plane;

	// ������� ��������������
    double _mes;

	// ����������� ����� �� ���������� ������ A, ������� �� ��������� plane
	// count - ���������� ����� � ������� A
	// ���� need_sort = "������", �� ������� A ����� ��������������� �� ������� �������
	Polygon(const Vertex* A, int count, const Plane& plane, bool need_sort);

public:

	Polygon(void);

	// ����������� ����� �� ���������� ������
	Polygon(const std::vector<Vertex> vertices);

	// ����������� ����� �� ���������� ������
	Polygon(const Vertex* A, int count);

	bool operator == (const Polygon& P) const;

	bool operator != (const Polygon& P) const;

	// ���������� �������� "������", ���� ��� ray ���������� �������������, ����� ���������� �������� "����" 
    bool intersects(const Ray& ray, Vertex& R) const;

	// ���������� �������� "������", ���� ������� AB ������������ � ���������������, ����� ���������� �������� "����" 
    bool intersects(const Vertex& A, const Vertex& B, Vertex& R) const;

	// ���������� ������, ���� ������� �������� �������������� ������������ �� ������ ��������������� 
	bool intersects(const Polygon& polygon) const;

	// ��������� ������������� ���������� plane �� ��� ��������������
	// plane - ��������� ������� ���������
	// A - ������ ������ ������� ��������������, ������������� �� ������� �������
	// B - ������ ������ ������� �������������� (���� �������������)
	// D - ������ ����� ������ ���� ��������������� A � B
	// count[0], count[1], count[2] - ���������� ���������� ������, ��������������, � ������� A, B, D
	int halve(const Plane& plane, Vertex* A, Vertex* B, Vertex* D, int* count) const;	

	// ���������� ��������� ���������, �� ������� ����� �������������
	Plane plane() const;

	// ���������� ������� ��������������
	double mes() const;

	// ���������� ���������� ������ ��������������
	Vertex center() const;

	// ���������� ���������� ������� �������������� � �������� �������� index
	Vertex vertex(int index) const;

	// ���������� ���������� ������ ��������������
	unsigned int size() const;

	// ���������� �������� "������", ���� ������� �������������� V ����������� �� ������� ������� ������������ ������� ������� (plane.A, plane.B, plane.C), ��� "����" � ��������� ������
	// ������ ������� ������������ ��� ��������� �������������� � OpenGL
	bool isClockwise(void) const;

	// ������� � ������� ������ ������ ��������������
	void print(void);

	~Polygon(void);

protected:


private:

	// ������� ���������� ������� �������������� 
	static double get_mes(const Vertex* V, unsigned int size);

	// ������������ ��������� ������� �� ������� �������
	void sort_vertices();

	void init(void);

	// ������� ���������� �������� "������", ���� ����� A ��������� ������ ��������������, ����� "����"
	bool inside(const Vertex& A) const;

	// ������� ���������� ������� � ��������������� ������� ������
	void deleteEquals();
};


} // namespace bstu


#endif // BSTU_POLYGON_H
