// Header Files
//=============

#include "cVector.h"

#include <cmath>
#include "../Asserts/Asserts.h"
#include "Functions.h"

// Static Data Initialization
//===========================

namespace
{
	const float s_epsilon = 1.0e-10f;
}

const eae6320::Math::cVector eae6320::Math::cVector::right(1.0f, 0.0f, 0.0f);
const eae6320::Math::cVector eae6320::Math::cVector::up(0.0f, 1.0f, 0.0f);
const eae6320::Math::cVector eae6320::Math::cVector::forward(0.0f, 0.0f, 1.0f);

const eae6320::Math::cVector eae6320::Math::cVector::left(-1.0f, 0.0f, 0.0f);
const eae6320::Math::cVector eae6320::Math::cVector::down(0.0f, -1.0f, 0.0f);
const eae6320::Math::cVector eae6320::Math::cVector::back(0.0f, 0.0f, -1.0f);

const eae6320::Math::cVector eae6320::Math::cVector::zero(0.0f, 0.0f, 0.0f);

// Interface
//==========

// Addition
eae6320::Math::cVector eae6320::Math::cVector::operator +(const cVector& i_rhs) const
{
	return cVector(x + i_rhs.x, y + i_rhs.y, z + i_rhs.z);
}
eae6320::Math::cVector& eae6320::Math::cVector::operator +=(const cVector& i_rhs)
{
	x += i_rhs.x;
	y += i_rhs.y;
	z += i_rhs.z;
	return *this;
}

// Subtraction / Negation
eae6320::Math::cVector eae6320::Math::cVector::operator -(const cVector& i_rhs) const
{
	return cVector(x - i_rhs.x, y - i_rhs.y, z - i_rhs.z);
}
eae6320::Math::cVector& eae6320::Math::cVector::operator -=(const cVector& i_rhs)
{
	x -= i_rhs.x;
	y -= i_rhs.y;
	z -= i_rhs.z;
	return *this;
}
eae6320::Math::cVector eae6320::Math::cVector::operator -() const
{
	return cVector(-x, -y, -z);
}

// Multiplication
eae6320::Math::cVector eae6320::Math::cVector::operator *(const float i_rhs) const
{
	return cVector(x * i_rhs, y * i_rhs, z * i_rhs);
}
eae6320::Math::cVector& eae6320::Math::cVector::operator *=(const float i_rhs)
{
	x *= i_rhs;
	y *= i_rhs;
	z *= i_rhs;
	return *this;
}

eae6320::Math::cVector eae6320::Math::operator *(const float i_lhs, const cVector& i_rhs)
{
	return i_rhs * i_lhs;
}

// Division
eae6320::Math::cVector eae6320::Math::cVector::operator /(const float i_rhs) const
{
	EAE6320_ASSERTF(std::abs(i_rhs) > s_epsilon, "Can't divide by zero");
	const float rhs_reciprocal = 1.0f / i_rhs;
	return cVector(x * rhs_reciprocal, y * rhs_reciprocal, z * rhs_reciprocal);
}
eae6320::Math::cVector& eae6320::Math::cVector::operator /=(const float i_rhs)
{
	EAE6320_ASSERTF(std::abs(i_rhs) > s_epsilon, "Can't divide by zero");
	const float rhs_reciprocal = 1.0f / i_rhs;
	x *= rhs_reciprocal;
	y *= rhs_reciprocal;
	z *= rhs_reciprocal;
	return *this;
}

// Length / Normalization
float eae6320::Math::cVector::GetLength() const
{
	return sqrtf(SqrGetLength());
}
float eae6320::Math::cVector::SqrGetLength() const
{
	return x*x + y*y + z*z;
}
void eae6320::Math::cVector::Normalize()
{
	*this = CreateNormalized();
}

eae6320::Math::cVector eae6320::Math::cVector::CreateNormalized() const
{
	const float length = GetLength();
	if (length < s_epsilon)return zero;
	{
		const float length_reciprocal = 1.0f / length;
		return cVector(x * length_reciprocal, y * length_reciprocal, z * length_reciprocal);
	}
}

eae6320::Math::cVector eae6320::Math::cVector::ClampMagnitude(const cVector& i_vector, const float i_maxLength)
{
	const float currentLength = i_vector.GetLength();
	if (currentLength > s_epsilon) return i_vector*(i_maxLength / currentLength);
	return  i_vector;
}

float eae6320::Math::cVector::DistanceBetween(const cVector & i_other) const
{
	return sqrtf(
		(x - i_other.x)*(x - i_other.x) +
		(y - i_other.y)*(y - i_other.y) +
		(z - i_other.z)*(z - i_other.z));
}

// Interpolation
eae6320::Math::cVector eae6320::Math::cVector::Slerp(const cVector i_start, const cVector i_end, const float i_t)
{
	EAE6320_ASSERTF((i_t >= 0.0f && i_t <= 1.0f), "Specify t between [0,100]");
	// Dot product - the cosine of the angle between 2 vectors.
	float dot = Dot(i_start.CreateNormalized(), i_end.CreateNormalized());
	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = Clamp(dot, -1.0f, 1.0f);
	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = acosf(dot)*i_t;
	cVector relativeVector = i_end - i_start*dot;
	relativeVector = relativeVector.CreateNormalized();// Orthonormal basis							 
	return i_start*cosf(theta) + relativeVector*sinf(theta);// The final result.

	/*const float dot = Dot(i_start, i_end);
	const float alpha = ConvertDegreesToRadians(dot);
	const float theta = acosf(alpha);
	const cVector t1 = (sinf(1.0f - i_t)*theta / sinf(alpha))*i_start;
	const cVector t2 = (sinf(i_t)*theta / sinf(alpha))*i_end;
	return  t1 + t2;*/
}
// Products
float eae6320::Math::Dot(const cVector& i_lhs, const cVector& i_rhs)
{
	return (i_lhs.x * i_rhs.x) + (i_lhs.y * i_rhs.y) + (i_lhs.z * i_rhs.z);
}
eae6320::Math::cVector eae6320::Math::Cross(const cVector& i_lhs, const cVector& i_rhs)
{
	return cVector(
		(i_lhs.y * i_rhs.z) - (i_lhs.z * i_rhs.y),
		(i_lhs.z * i_rhs.x) - (i_lhs.x * i_rhs.z),
		(i_lhs.x * i_rhs.y) - (i_lhs.y * i_rhs.x));
}

// Comparison
bool eae6320::Math::cVector::operator ==(const cVector& i_rhs) const
{
	// Use & rather than && to prevent branches (all three comparisons will be evaluated)
	return (x == i_rhs.x) & (y == i_rhs.y) & (z == i_rhs.z);
}
bool eae6320::Math::cVector::operator !=(const cVector& i_rhs) const
{
	// Use | rather than || to prevent branches (all three comparisons will be evaluated)
	return (x != i_rhs.x) | (y != i_rhs.y) | (z != i_rhs.z);
}

// Initialization / Shut Down
//---------------------------

eae6320::Math::cVector::cVector(const float i_x, const float i_y, const float i_z)
	:
	x(i_x), y(i_y), z(i_z)
{

}

eae6320::Math::cVector::cVector(const cVector & i_other)
	:
	x(i_other.x), y(i_other.y), z(i_other.z)
{

}

eae6320::Math::cVector& eae6320::Math::cVector::operator=(const cVector& i_other)
{
	x = i_other.x;
	y = i_other.y;
	z = i_other.z;
	return *this;
}
