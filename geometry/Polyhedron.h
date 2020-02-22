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
        // ������������� �������������
        int ID;

        std::vector<VolumePolygon*> polygons;

        // �������� �������������
        Polyhedron* C1;

        Polyhedron* C2;

        // ����� �������������
        double _mes;

        // ������������� ���������
        int visit_id;

        Vertex _center;

private:

        double get_mes() const;

        Vertex get_center() const;

        // �������������� ������� �������� � ���� ������
        void right_normals(void);

public:
	
	Polyhedron(void);

	// ����������� ������������� �� ���������� ������
	Polyhedron(VolumePolygon** P, int count);

	// ���������� ��������� �� ������������� � �������� ������� index
	VolumePolygon* polygon(int index) const;

	// ���������� ���������� ��������������� � �������������
	int size() const;

	// ���������� ����� �������������
	double volume() const;

	// ���������� ���������� ������ �������������
	Vertex center() const;

	// ���������� �������� "������", ���� ������������� polygon ������������ � �������������� ��� ��������� ������ ����
	bool intersects(const Polygon& polygon) const;

	// ���������� �������� "������", ���� ������������ ������������ � �����, ����� ���������� �������� "����"
	bool intersects(const Ray& ray, Vertex& R) const;

	// ������� ���������� �������� "������", ���� ����� ��������� ������ �������������, ����� ���������� �������� "����"
	bool inside(const Vertex& vertex) const;

	bool halve(const Plane& halver);

	bool halve(Halver& halver);
	
	// ���������� �������� "������", ���� ������������ �������, �.�. �������� �������� �������������, ����� ���������� �������� "����"
	bool isHalve() const;

	// ����������� ����� � ��������� ���������������
	void link(void);

	// ���������� ��������� �� ������ �������� ������������
	Polyhedron* child1() const;

	// ���������� ��������� �� ������ �������� ������������
	Polyhedron* child2() const;

	// ���������� ������������� ������ (visit_id) � ���� ��������������, � ��� ����� ��������
	void resetVisit(void);

	void dispose_childs(void);

	// ������� ��������� � ������ ��� ��������������-������ �������� �������������
	int collectLowers(VolumePolygon** P);

	// ��������� ������������ ��� ������������ ������ ������ ��������������
	template<class A> void forEach(A & action);

	~Polyhedron(void);

	// ���������� ���������� ��������������, ��������� � ������������ ������ (������������ ��� �������)
	static int alloc_count();

	double check_mes();

	// �������� ��� ������ ���� �������� ��������������
	class full_walker
	{
		Polyhedron* P;
		std::stack<Polyhedron*> st;
	  public:
		full_walker(Polyhedron* start); // �����������
		operator Polyhedron*() const;  // ���������� ��������� ��������������
		Polyhedron* operator*() const; // ���������� � ���� Polyhedron*
		void operator++(void); // ������� � ���������� �������������
	};

	// �������� ��� ������ ������ ��������������-�������
	class low_walker
	{
		Polyhedron* P;
		std::stack<Polyhedron*> st;
	  public:
		low_walker(Polyhedron* start); // �����������
		operator Polyhedron*() const;  // ���������� ��������� ��������������
		Polyhedron* operator*() const; // ���������� � ���� Polyhedron*
		void operator++(void); // ������� � ���������� �������������
	};

	template <class Walker> friend void deepWalk(Polyhedron& p2, Walker &, int);

};

} // namespace bstu

#endif // BSTU_POLYHEDRON_H
