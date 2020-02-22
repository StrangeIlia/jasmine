#ifndef BSTU_VECTOR_H
#define BSTU_VECTOR_H

#include "lib_geometry_global.h"

namespace bstu
{

class LIB_GEOMETRYSHARED_EXPORT Vector
{
public:
	
	double x, y, z;

	Vector(void);

	Vector(double x, double y, double z);
	
	Vector operator + (const Vector& V) const;

	// Функция возвращает скалярное произведение векторов
	double operator * (const Vector& V) const;

	// Возвращает длину вектора
	double length() const;

	Vector operator / (double divisor) const;

	Vector operator * (double scale) const;

	// Возвращает векторное произведение векторов
	static Vector crossProduct(const Vector &A, const Vector &B);

	~Vector(void);
};



} // namespace bstu


#endif // BSTU_VECTOR_H
