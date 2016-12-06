/*
This class represents a half(half of float)
*/

#ifndef EAE6320_MATH_CHALF_H
#define EAE6320_MATH_CHALF_H

#include <cstdint>

namespace eae6320
{
	namespace Math
	{
		class cHalf
		{
		public:
			static uint16_t MakeHalfFromFloat(float value);
			static float MakeFloatFromHalf(uint16_t value);
		private:
			union Bits
			{
				float f;
				int32_t si;
				uint32_t ui;
			};
		};
	}
}

#endif //  EAE6320_MATH_CHALF_H