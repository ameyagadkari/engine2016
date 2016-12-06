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
			bool IsBitSet(uint8_t number, uint8_t bitNumber)
			{
				uint8_t bit_mask = 0x80;
				return (number & (bit_mask >> bitNumber)) != 0;
			}
			bool IsBitClear(uint8_t number, uint8_t bitNumber)
			{
				return !IsBitSet(number, bitNumber);
			}
			void SetBit(uint8_t& number, uint8_t bitNumber)
			{
				uint8_t bit_mask = 0x80;
				number = number | (bit_mask >> bitNumber);
			}
			void ClearBit(uint8_t& number, uint8_t bitNumber)
			{
				uint8_t bit_mask = 0x80;
				number = number & ~(bit_mask >> bitNumber);
			}
		}
	}
}

#endif // !EAE6320_MATH_BIT_MANIPULATOR_H
