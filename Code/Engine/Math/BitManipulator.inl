#ifndef EAE6320_MATH_BIT_MANIPULATOR_INL
#define EAE6320_MATH_BIT_MANIPULATOR_INL

namespace
{
	const uint8_t bit_mask = 0x80;
}

inline bool eae6320::Math::BitManipulator::IsBitSet(uint8_t number, uint8_t bitNumber)
{	
	return (number & (bit_mask >> bitNumber)) != 0;
}
inline bool eae6320::Math::BitManipulator::IsBitClear(uint8_t number, uint8_t bitNumber)
{
	return !IsBitSet(number, bitNumber);
}
inline void eae6320::Math::BitManipulator::SetBit(uint8_t& number, uint8_t bitNumber)
{
	number = number | (bit_mask >> bitNumber);
}
inline void eae6320::Math::BitManipulator::ClearBit(uint8_t& number, uint8_t bitNumber)
{
	number = number & ~(bit_mask >> bitNumber);
}

#endif // !EAE6320_MATH_BIT_MANIPULATOR_INL