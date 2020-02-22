#ifndef BSTU_VERTEX_H
#define BSTU_VERTEX_H

#include "Vector.h"
#include "lib_geometry_global.h"

namespace bstu
{

class LIB_GEOMETRYSHARED_EXPORT Vertex
{
  public:

	float x, y, z;

	Vertex() {}

	Vertex(float _x, float _y, float _z);

	Vertex(const Vertex& vertex);

	bool operator == (const Vertex& vertex) const;

	bool operator != (const Vertex& vertex) const;

	// ������� ���������� ������� ���������� ����� �������
	double R2(const Vertex& A) const;

	// ������� ���������� ���������� ����� ������� 
	double distanceTo(const Vertex& A) const;

	// ���������� �������� "������", ���� ������� �����
	bool equals(const Vertex& A) const;

	Vector operator + (const Vertex& A) const;

	Vector operator - (const Vertex& A) const;

	operator Vector() const;
};


} // namespace bstu


#endif // BSTU_VERTEX_H
