#ifndef BSTU_PLANE_H
#define BSTU_PLANE_H

#include "Vertex.h"
#include "Ray.h"
#include "lib_geometry_global.h"

namespace bstu
{

// ��������� ������� ���������� A*x + B*y + C*z + D = 0
class LIB_GEOMETRYSHARED_EXPORT Plane
{
public:

  static const Plane Empty;

  double A, B, C, D;

  Plane();

  // ����������� ��������� �� ��� ������
  Plane(const Vertex& V0, const Vertex& V1, const Vertex& V2);

  // ����������� ��������� �� ���������� ������, ������� ������� �� ����� ���������
  Plane(Vertex* V, int nVertices);

  Plane(double A, double B, double C, double D);

  double distanceTo(const Vertex& vertex) const;

  // ������� ���������� ����� ����������� ��������� � ����� ray
  Vertex intersects(const Ray& ray) const;

  // ���������� �������� "������", ���� ������� ��������� ������������ � �������� AB
  bool intersects(const Vertex& A, const Vertex& B) const;

  bool operator == (const Plane& plane) const;

  Plane operator -() const;

  // ������������ ������ ������� �� 180 ��������
  void turn_back(void);

  // ���������� �������� "������", ���� ��������� ��������� �������� �����, ����� ���������� �������� "����"  
  bool isEmpty() const;

};

} // namespace bstu

#endif // BSTU_PLANE_H
