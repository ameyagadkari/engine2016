// Header Files
//=============

#include "Functions.h"

// Static Data Initialization
//===========================

const float eae6320::Math::Pi = 3.141592654f;

// Interface
//==========

float eae6320::Math::ConvertDegreesToRadians( const float i_degrees )
{
	return i_degrees * Pi / 180.0f;
}

float eae6320::Math::ConvertRadiansToDegrees(const float i_radians)
{
	return i_radians * 180.0f / Pi;
}

float eae6320::Math::LerpDegrees(const float i_start, const float i_end, const float i_amount)
{
	float num = Repeat(i_end - i_start, 360.0f);
	if (num > 180.0f)num -= 360.0f;
	return i_start + num * Clamp01(i_amount);
}

float eae6320::Math::Repeat(const float i_value, const float i_length)
{
	return i_value - floorf(i_value / i_length) * i_length;
}

float eae6320::Math::Clamp01(const float i_value)
{
	if (i_value < 0.0)return 0.0f;
	if (i_value > 1.0)return 1.0f;
	return i_value;
}