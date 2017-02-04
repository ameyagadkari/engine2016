#ifndef EAE6320_COLOR_H
#define EAE6320_COLOR_H

namespace eae6320
{
	namespace Debug
	{
		struct Color
		{
			float r, g, b;
			Color(float i_r, float i_g, float i_b) :
				r(i_r), g(i_g), b(i_b)
			{}
		};
	}
}

#endif // !EAE6320_COLOR_H