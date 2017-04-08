/*
	This file contains math-related functions
*/

#ifndef EAE6320_MATH_FUNCTIONS_H
#define EAE6320_MATH_FUNCTIONS_H
#include <cfloat>

namespace eae6320
{
	namespace Math
	{
		// Interface
		//==========

		extern const float Pi;

		float ConvertDegreesToRadians(const float i_degrees);
		float ConvertRadiansToDegrees(const float i_radians);
		float LerpDegrees(const float i_start, const float i_end, const float i_amount);
		float Repeat(const float i_value, const float i_length);
		float Clamp01(const float i_value);
		template<typename tUnsignedInteger>
		tUnsignedInteger RoundUpToMultiple(const tUnsignedInteger i_value, const tUnsignedInteger i_multiple);
		template<typename tUnsignedInteger>
		tUnsignedInteger RoundUpToMultiple_powerOf2(const tUnsignedInteger i_value,
			const tUnsignedInteger i_multipleWhichIsAPowerOf2);
		template<typename T>
		T Clamp(const T i_value, const T i_min, const T i_max);
		inline bool AlmostEqualUlpsAndAbs(float A, float B, float maxDiff = 4 * 1024 * 1024, int maxUlpsDiff = FLT_EPSILON);
	}
}

#include "Functions.inl"

#endif	// EAE6320_MATH_FUNCTIONS_H
