#ifndef EAE6320_PIXELCOORDINATES_H
#define EAE6320_PIXELCOORDINATES_H
#include <cstdint>

namespace eae6320
{
	namespace Debug
	{
		struct PixelCoordinates
		{
			int16_t x, y;
			PixelCoordinates(int16_t i_x, int16_t i_y) :
				x(i_x), y(i_y)
			{}
		};
	}
}

#endif // !EAE6320_PIXELCOORDINATES_H