// Header Files
//=============

#include "cQuaternion.h"

#include <cmath>
#include "cVector.h"
#include "../Asserts/Asserts.h"

// Static Data Initialization
//===========================

namespace
{
	const float s_epsilon = 1.0e-9f;
}

// Interface
//==========

// Concatenation
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator *(const cQuaternion& i_rhs) const
{
	return cQuaternion(
		(m_w * i_rhs.m_w) - ((m_x * i_rhs.m_x) + (m_y * i_rhs.m_y) + (m_z * i_rhs.m_z)),
		(m_w * i_rhs.m_x) + (m_x * i_rhs.m_w) + ((m_y * i_rhs.m_z) - (m_z * i_rhs.m_y)),
		(m_w * i_rhs.m_y) + (m_y * i_rhs.m_w) + ((m_z * i_rhs.m_x) - (m_x * i_rhs.m_z)),
		(m_w * i_rhs.m_z) + (m_z * i_rhs.m_w) + ((m_x * i_rhs.m_y) - (m_y * i_rhs.m_x)));
}

// Inversion
void eae6320::Math::cQuaternion::Invert()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
}
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::CreateInverse() const
{
	return cQuaternion(m_w, -m_x, -m_y, -m_z);
}

// Normalization
void eae6320::Math::cQuaternion::Normalize()
{
	const float length = sqrt((m_w * m_w) + (m_x * m_x) + (m_y * m_y) + (m_z * m_z));
	EAE6320_ASSERTF(length > s_epsilon, "Can't divide by zero");
	const float length_reciprocal = 1.0f / length;
	m_w *= length_reciprocal;
	m_x *= length_reciprocal;
	m_y *= length_reciprocal;
	m_z *= length_reciprocal;
}
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::CreateNormalized() const
{
	const float length = sqrt((m_w * m_w) + (m_x * m_x) + (m_y * m_y) + (m_z * m_z));
	EAE6320_ASSERTF(length > s_epsilon, "Can't divide by zero");
	const float length_reciprocal = 1.0f / length;
	return cQuaternion(m_w * length_reciprocal, m_x * length_reciprocal, m_y * length_reciprocal, m_z * length_reciprocal);
}

// Products
float eae6320::Math::Dot(const cQuaternion& i_lhs, const cQuaternion& i_rhs)
{
	return (i_lhs.m_w * i_rhs.m_w) + (i_lhs.m_x * i_rhs.m_x) + (i_lhs.m_y * i_rhs.m_y) + (i_lhs.m_z * i_rhs.m_z);
}

// Initialization / Shut Down
//---------------------------

eae6320::Math::cQuaternion::cQuaternion()
	:
	m_w(1.0f), m_x(0.0f), m_y(0.0f), m_z(0.0f)
{

}

eae6320::Math::cQuaternion::cQuaternion(const float i_angleInRadians, const cVector& i_axisOfRotation_normalized)
{
	const float theta_half = i_angleInRadians * 0.5f;
	m_w = std::cos(theta_half);
	const float sin_theta_half = std::sin(theta_half);
	m_x = i_axisOfRotation_normalized.x * sin_theta_half;
	m_y = i_axisOfRotation_normalized.y * sin_theta_half;
	m_z = i_axisOfRotation_normalized.z * sin_theta_half;
}

// Implementation
//===============

// Initialization / Shut Down
//---------------------------

eae6320::Math::cQuaternion::cQuaternion(const float i_w, const float i_x, const float i_y, const float i_z)
	:
	m_w(i_w), m_x(i_x), m_y(i_y), m_z(i_z)
{

}

// Negation
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator-()const
{
	return cQuaternion(-m_w, -m_x, -m_y, -m_z);
}

// Addition
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator+(const cQuaternion & i_rhs) const
{
	return cQuaternion(m_w + i_rhs.m_w, m_x + i_rhs.m_x, m_y + i_rhs.m_y, m_z + i_rhs.m_z);
}

// Multiplication
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator*(float i_rhs) const
{
	return cQuaternion(m_w * i_rhs, m_x * i_rhs, m_y * i_rhs, m_z * i_rhs);
}

// Division
eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator/(float i_rhs) const
{
	EAE6320_ASSERTF(std::abs(i_rhs) > s_epsilon, "Can't divide by zero");
	const float rhs_reciprocal = 1.0f / i_rhs;
	return cQuaternion(m_w * rhs_reciprocal, m_x * rhs_reciprocal, m_y * rhs_reciprocal, m_z * rhs_reciprocal);
}

// Interpolation

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::Slerp(const cQuaternion& q1, const cQuaternion& q2, float t)
{
	cQuaternion q3;
	float dot = Dot(q1, q2);

	/*	dot = cos(theta)
	if (dot < 0), q1 and q2 are more than 90 degrees apart,
	so we can invert one to reduce spinning	*/
	if (dot < 0.0f)
	{
		dot = -dot;
		q3 = -q2;
	}
	else q3 = q2;

	if (dot < 0.95f)
	{
		float angle = acosf(dot);
		return (q1*sinf(angle*(1 - t)) + q3*sinf(angle*t)) / sinf(angle);
	}
	return Lerp(q1, q3, t);// if the angle is small, use linear interpolation
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::Lerp(const cQuaternion& q1, const cQuaternion& q2, float t)
{
	return (q1*(1 - t) + q2*t).CreateNormalized();
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::SlerpNoInvert(const cQuaternion& q1, const cQuaternion& q2, float t)
{
	float dot = Dot(q1, q2);

	if (dot > -0.95f && dot < 0.95f)
	{
		float angle = acosf(dot);
		return (q1*sinf(angle*(1 - t)) + q2*sinf(angle*t)) / sinf(angle);
	}
	return Lerp(q1, q2, t);  // if the angle is small, use linear interpolation	
}