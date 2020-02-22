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
	// ��������� �� �������� ��������������
	VolumePolygon* C1,* C2;
	
	// ��������� �� ���������� �������������
	VolumePolygon* L;

	// ������������, �������� ����������� ������� �������������
	Polyhedron* T;

	// �����������, ������� ����������� �������������
	Surface* S;

private:

	void init(void);

	// ����������� ����� �� ���������� ������ A, ������� �� ��������� plane
	VolumePolygon(const Vertex* A, int count, const Plane& plane, bool need_sort);

public:
																									
	VolumePolygon(void);

	VolumePolygon(const VolumePolygon* polygon);

	// ����������� ����� �� ���������� ������
	VolumePolygon(const std::vector<Vertex> vertices);

	// ����������� ����� �� ���������� ������
	VolumePolygon(const Vertex* A, int count);

	// ��������� ������������� ���������� �� ��� ��������������.
	// ���������� �������� "������", ���� ������� ������� �������������, ����� ���������� �������� "����"
	// ��������� � ��������� halve_data ���������� � ���������� ��������������
	bool halve(Halver& halve_data);

	static bool finishHalf(Halver& halve_data);

	// ���������� �������� "������", ���� ������������� ������� �������, ����� ���������� �������� "����"
	bool isHalve();

	// ���������� ������ �������� ������������
	VolumePolygon* child1() const;

	// ���������� ������ �������� ������������
	VolumePolygon* child2() const;

	// ���������� �������� ������������
	Polyhedron* neighbor() const;

	// ��������� ��������� �������� �������������� �������� �������������� � ���������� ��������������� L (� ������ ��������, ���� �� ��� ���)
	void link();

	void dispose(void);

	~VolumePolygon(void);

	// ���������� ���������� ���������������, ����������� � ������������ ������ (������������ ��� �������)
	static int alloc_count(); 

	friend class Halver;

	friend class Polyhedron;

	template <class Walker> friend void deepWalk(Polyhedron& p2, Walker &, int);				
	
	void check();

    // �������� ��� ������ ���� �������� ���������������
	class walker
	{
		VolumePolygon* current;
		std::stack<VolumePolygon*> st;
	  public:
		walker(VolumePolygon* start); // �����������
		operator VolumePolygon*() const;  // ���������� ��������� ��������������
		VolumePolygon* operator*() const; // ���������� � ���� VolumePolygon*
		void operator++(void); // ������� � ���������� ��������������
	};
};

} // namespace bstu


#endif // BSTU_VOLUME_POLYGON_H
