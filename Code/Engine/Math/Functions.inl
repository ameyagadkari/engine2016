#ifndef EAE6320_MATH_FUNCTIONS_INL
#define EAE6320_MATH_FUNCTIONS_INL

// Header Files
//=============

#include "../Asserts/Asserts.h"
#include <cstdint>
#include <cmath>
// Helper Struct
//=============
union Float_t
{
	explicit Float_t(float num = 0.0f) : f(num) {}
	// Portable extraction of components.
	bool Negative() const { return i < 0; }
	int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
	int32_t RawExponent() const { return (i >> 23) & 0xFF; }

	int32_t i;
	float f;
#ifdef _DEBUG
	struct
	{   // Bitfields for exploration. Do not use in production code.
		uint32_t mantissa : 23;
		uint32_t exponent : 8;
		uint32_t sign : 1;
	} parts;
#endif
};

// Implementation
//===============

template<typename tUnsignedInteger>
tUnsignedInteger eae6320::Math::RoundUpToMultiple(const tUnsignedInteger i_value, const tUnsignedInteger i_multiple)
{
	EAE6320_ASSERTF(i_multiple != 0, "Zero isn't a valid multiple");
	EAE6320_ASSERTF(i_multiple > 0, "The multiple must be positive");
	// Adding one less than the multiple will make the value at or above the next hiehst multiple
	// unless the value was itself a multiple.
	// Dividng and multiplying by the multiple removes any remainder
	const tUnsignedInteger returnValue = ((i_value + i_multiple - 1) / i_multiple) * i_multiple;
	EAE6320_ASSERT((returnValue % i_multiple) == 0);
	return returnValue;
}

template<typename tUnsignedInteger>
inline tUnsignedInteger eae6320::Math::RoundUpToMultiple_powerOf2(const tUnsignedInteger i_value,
	const tUnsignedInteger i_multipleWhichIsAPowerOf2)
{
	EAE6320_ASSERTF(i_multipleWhichIsAPowerOf2 != 0, "Zero isn't a valid multiple");
	EAE6320_ASSERTF(i_multipleWhichIsAPowerOf2 > 0, "The multiple must be positive");
	// To be a power-of-2 the multiple can only have a single bit set;
	// get a mask of the bits less-significant than that single bit
	const tUnsignedInteger nonLeadingBits = i_multipleWhichIsAPowerOf2 - 1;
	EAE6320_ASSERTF((i_multipleWhichIsAPowerOf2 && (i_multipleWhichIsAPowerOf2 & nonLeadingBits)) == 0, "The multiple must be a power-of-2");
	// Adding the non-leading bits will make the value at or above the next highest multiple
	// unless the value was itself a multiple.
	// ANDing with inverse then removes any bits less than the multiple.
	const tUnsignedInteger returnValue = (i_value + nonLeadingBits) & ~nonLeadingBits;
	EAE6320_ASSERT((returnValue % i_multipleWhichIsAPowerOf2) == 0);
	return returnValue;
}

template<typename T>
T eae6320::Math::Clamp(const T i_value, const T i_min, const T i_max)
{
	return (i_value < i_min) ? i_min : ((i_value > i_max) ? i_max : i_value);
}

inline bool eae6320::Math::AlmostEqualUlpsAndAbs(float A, float B, float maxDiff, int maxUlpsDiff)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	float absDiff = fabs(A - B);
	if (absDiff <= maxDiff) return true;

	Float_t uA(A);
	Float_t uB(B);

	// Different signs means they do not match.
	if (uA.Negative() != uB.Negative()) return false;

	// Find the difference in ULPs.
	int ulpsDiff = abs(uA.i - uB.i);
	if (ulpsDiff <= maxUlpsDiff) return true;

	return false;
}
#endif	// EAE6320_MATH_FUNCTIONS_INL