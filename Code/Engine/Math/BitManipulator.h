/*
This file provides utilites for bit manipulation in a byte of data
*/

#ifndef EAE6320_MATH_BIT_MANIPULATOR_H
#define EAE6320_MATH_BIT_MANIPULATOR_H

#include <cstdint>

namespace eae6320
{
	namespace Math
	{
		namespace BitManipulator
		{
			bool IsBitSet(uint8_t number, uint8_t bitNumber);
			bool IsBitClear(uint8_t number, uint8_t bitNumber);
			void SetBit(uint8_t& number, uint8_t bitNumber);
			void ClearBit(uint8_t& number, uint8_t bitNumber);
		}
	}
}

#include "BitManipulator.inl"

#endif // !EAE6320_MATH_BIT_MANIPULATOR_H
