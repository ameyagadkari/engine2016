#ifndef EAE6320_FONTDATA_H
#define EAE6320_FONTDATA_H

#include<cstdint>

namespace eae6320
{
	namespace Graphics
	{
		struct FontData
		{
			struct sFont
			{
				float uleft, uright;
				uint8_t width;
			};
			sFont *m_fontData;
			uint8_t m_numberOfCharacters;
			FontData() :
				m_fontData(nullptr),
				m_numberOfCharacters(0)
			{}
			FontData(const uint8_t i_numberOfCharacters) :
				m_fontData(reinterpret_cast<sFont*>(malloc(i_numberOfCharacters * sizeof(sFont)))),
				m_numberOfCharacters(i_numberOfCharacters)
			{}
			~FontData()
			{
				if (m_fontData)
				{
					free(m_fontData);
					m_fontData = nullptr;
				}
			}
		};
	}
}
#endif // !EAE6320_FONTDATA_H
