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
	// ������� ���������
	const Plane _plane;

    Vertex V[128]; 
	
	// P1[0] � P2[0] - ��������������, ���������� � ���������� ���������� ���������� plane.
	// ��������� �������� � ���������� ���������� ������������ ������� ���������.
	// P1[i] (i=1, n1-1) ����������� ���� ������� ���������, P2[i] (i=1, n2-1) - ���� ������� ���������.
    VolumePolygon * P1[128], * P2[128];

    int n1, n2, n;

public:

	Polyhedron* T1, * T2;

    Halver(const Plane& plane);

	Plane plane() const;

    // ��������� ���������, ���� �� ����� ������� � ������� V � ���� ���, �� ��������� � � ���� ������
    void add(const Vertex& vertex);

    // �������� ������������, ������� ���� ���������
    void addUpper(VolumePolygon* p);

    // �������� ������������, ������� ���� ���������
    void addLower(VolumePolygon* p);

	void reset(void);

	// ������� ������ ��� �������������� P[0] � P[1], ��������� �� ������ V
	// ���������� �������� "������", ���� �������������� ������� �������, �.�. ���������� ��������� ������ ������ ����, ����� ���������� �������� "����"
	bool finishHalf(void);

	// ��������� ���������� ��������, ������� ����� ��������� ��� ���������� ����������������
	virtual void withChild(VolumePolygon* child);

	// ����������, ���������� �� ������ ������������?
	virtual bool needHalve(Polyhedron* P) const;
};


} // namespace bstu


#endif // BSTU_HALVEDATA_H
