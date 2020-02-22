#ifndef BSTU_PLANE_H
#define BSTU_PLANE_H

#include "Vertex.h"
#include "Ray.h"
#include "lib_geometry_global.h"

namespace bstu
{

// ѕлоскость задаЄтс€ уравнением A*x + B*y + C*z + D = 0
class LIB_GEOMETRYSHARED_EXPORT Plane
{
public:

  static const Plane Empty;

  double A, B, C, D;

  Plane();

  //  онструктор плоскости по трЄм точкам
  Plane(const Vertex& V0, const Vertex& V1, const Vertex& V2);

  //  онструктор плоскости по нескольким точкам, условно лежащим на одной плоскости
  Plane(Vertex* V, int nVertices);

  Plane(double A, double B, double C, double D);

  double distanceTo(const Vertex& vertex) const;

  // ‘ункци€ возвращает точку пересечени€ плоскости с лучом ray
  Vertex intersects(const Ray& ray) const;

  // ¬озвращает значение "истина", если текуща€ плоскость пересекаетс€ с отрезком AB
  bool intersects(const Vertex& A, const Vertex& B) const;

  bool operator == (const Plane& plane) const;

  Plane operator -() const;

  // ѕоворачивает вектор нормали на 180 градусов
  void turn_back(void);

  // ¬озвращает значение "истина", если уравнение плоскости записано верно, иначе возвращает значение "ложь"  
  bool isEmpty() const;

};

} // namespace bstu

#endif // BSTU_PLANE_H
