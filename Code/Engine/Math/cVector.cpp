// Header Files
//=============

#include "cVector.h"

#include <cmath>
#include "../Asserts/Asserts.h"

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
eae6320::Math::cVector eae6320::Math::cVector::operator +( const cVector& i_rhs ) const
{
	return cVector( x + i_rhs.x, y + i_rhs.y, z + i_rhs.z );
}
eae6320::Math::cVector& eae6320::Math::cVector::operator +=( const cVector& i_rhs )
{
	x += i_rhs.x;
	y += i_rhs.y;
	z += i_rhs.z;
	return *this;
}

// Subtraction / Negation
eae6320::Math::cVector eae6320::Math::cVector::operator -( const cVector& i_rhs ) const
{
	return cVector( x - i_rhs.x, y - i_rhs.y, z - i_rhs.z );
}
eae6320::Math::cVector& eae6320::Math::cVector::operator -=( const cVector& i_rhs )
{
	x -= i_rhs.x;
	y -= i_rhs.y;
	z -= i_rhs.z;
	return *this;
}
eae6320::Math::cVector eae6320::Math::cVector::operator -() const
{
	return cVector( -x, -y, -z );
}

// Multiplication
eae6320::Math::cVector eae6320::Math::cVector::operator *( const float i_rhs ) const
{
	return cVector( x * i_rhs, y * i_rhs, z * i_rhs );
}
eae6320::Math::cVector& eae6320::Math::cVector::operator *=( const float i_rhs )
{
	x *= i_rhs;
	y *= i_rhs;
	z *= i_rhs;
	return *this;
}

eae6320::Math::cVector eae6320::Math::operator *( const float i_lhs, const cVector& i_rhs )
{
	return i_rhs * i_lhs;
}

// Division
eae6320::Math::cVector eae6320::Math::cVector::operator /( const float i_rhs ) const
{
	EAE6320_ASSERTF( std::abs( i_rhs ) > s_epsilon, "Can't divide by zero" );
	const float rhs_reciprocal = 1.0f / i_rhs;
	return cVector( x * rhs_reciprocal, y * rhs_reciprocal, z * rhs_reciprocal );
}
eae6320::Math::cVector& eae6320::Math::cVector::operator /=( const float i_rhs )
{
	EAE6320_ASSERTF( std::abs( i_rhs ) > s_epsilon, "Can't divide by zero" );
	const float rhs_reciprocal = 1.0f / i_rhs;
	x *= rhs_reciprocal;
	y *= rhs_reciprocal;
	z *= rhs_reciprocal;
	return *this;
}

// Length / Normalization
float eae6320::Math::cVector::GetLength() const
{
	return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
}

void eae6320::Math::cVector::Normalize()
{
	const float length = GetLength();
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	operator /=( length );
}

eae6320::Math::cVector eae6320::Math::cVector::CreateNormalized() const
{
	const float length = GetLength();
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	const float length_reciprocal = 1.0f / length;
	return cVector( x * length_reciprocal, y * length_reciprocal, z * length_reciprocal );
}

eae6320::Math::cVector eae6320::Math::cVector::ClampMagnitude(const float i_maxLength) const
{
	const float currentLength = GetLength();
	if(currentLength > s_epsilon) return *this*(i_maxLength / currentLength);
	return  *this;
}

float eae6320::Math::cVector::DistanceBetween(const cVector & i_other) const
{
	return sqrtf(
		(x - i_other.x)*(x - i_other.x) + 
		(y - i_other.y)*(y - i_other.y) + 
		(z - i_other.z)*(z - i_other.z));
}

// Products
float eae6320::Math::Dot( const cVector& i_lhs, const cVector& i_rhs )
{
	return ( i_lhs.x * i_rhs.x ) + ( i_lhs.y * i_rhs.y ) + ( i_lhs.z * i_rhs.z );
}
eae6320::Math::cVector eae6320::Math::Cross( const cVector& i_lhs, const cVector& i_rhs )
{
	return cVector(
		( i_lhs.y * i_rhs.z ) - ( i_lhs.z * i_rhs.y ),
		( i_lhs.z * i_rhs.x ) - ( i_lhs.x * i_rhs.z ),
		( i_lhs.x * i_rhs.y ) - ( i_lhs.y * i_rhs.x ) );
}

// Comparison
bool eae6320::Math::cVector::operator ==( const cVector& i_rhs ) const
{
	// Use & rather than && to prevent branches (all three comparisons will be evaluated)
	return ( x == i_rhs.x ) & ( y == i_rhs.y ) & ( z == i_rhs.z );
}
bool eae6320::Math::cVector::operator !=( const cVector& i_rhs ) const
{
	// Use | rather than || to prevent branches (all three comparisons will be evaluated)
	return ( x != i_rhs.x ) | ( y != i_rhs.y ) | ( z != i_rhs.z );
}

// Initialization / Shut Down
//---------------------------

eae6320::Math::cVector::cVector( const float i_x, const float i_y, const float i_z )
	:
	x( i_x ), y( i_y ), z( i_z )
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
