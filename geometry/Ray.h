#ifndef BSTU_RAY_H
#define BSTU_RAY_H

#include "Vertex.h"
#include "lib_geometry_global.h"

namespace bstu
{

// ����� - ���

class LIB_GEOMETRYSHARED_EXPORT Ray
{
  public:

	Vertex A; // ������ ����

	Vector N; // ����������� ����  

	Ray();

	// ����������� ���� �� ���� ������. ���� is_sort = "������", �� ����� A � B ���������������
	Ray(const Vertex& A, const Vertex& B, bool is_sort = false);

};

} // namespace bstu

#endif // BSTU_RAY_H
