#include "Font.h"

#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"

#include "FontData.h"
#include "Material.h"
#include "SpriteData.h"
#include "../Math/cHalf.h"

eae6320::Graphics::Material* eae6320::Graphics::Font::ms_material = NULL;
eae6320::Graphics::Sprite::TextureCoordinates* eae6320::Graphics::Font::texcoordArray = NULL;
uint8_t* eae6320::Graphics::Font::widthArray = NULL;

bool eae6320::Graphics::Font::LoadFont(const char * const i_relativePath)
{
	bool wereThereErrors = false;
	FontData *fontData = NULL;
	// Load the binary mesh file
	Platform::sDataFromFile binaryFontData;
	{
		std::string errorMessage;
		if (!LoadBinaryFile(i_relativePath, binaryFontData, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("Failed to load the binary font data file \"%s\": %s", i_relativePath, errorMessage.c_str());
			goto OnExit;
		}
	}
	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryFontData.data);

	fontData = reinterpret_cast<FontData*>(malloc(sizeof(FontData)));

	// Extracting Binary Data
	{
		// Extracting number of characters
		fontData->m_numberOfCharacters = *reinterpret_cast<uint32_t*>(data);

		// Extracting Font Array
		data += sizeof(uint8_t);
		fontData->m_fontData = reinterpret_cast<FontData::sFont*>(data);

		// Extracting Material File Path and loading material
		data += fontData->m_numberOfCharacters * sizeof(FontData::sFont);
		uint8_t offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		
		data += sizeof(offsetToAdd);
		{
			const char * const materialPath = reinterpret_cast<const char *>(data);
			ms_material = new Material();
			if (!Material::LoadMaterial(materialPath, *ms_material))
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				Logging::OutputError("Failed to load the binary effect file: %s", materialPath);
				goto OnExit;
			}
		}
	}

	texcoordArray = reinterpret_cast<Sprite::TextureCoordinates*>(malloc(fontData->m_numberOfCharacters * sizeof(Sprite::TextureCoordinates)));
	widthArray = reinterpret_cast<uint8_t*>(malloc(fontData->m_numberOfCharacters * sizeof(uint8_t)));
	for (size_t i = 0; i < fontData->m_numberOfCharacters; i++)
	{
		widthArray[i] = fontData->m_fontData[i].width;
		texcoordArray[i].left = Math::cHalf::MakeHalfFromFloat(fontData->m_fontData[i].uleft);
		texcoordArray[i].right = Math::cHalf::MakeHalfFromFloat(fontData->m_fontData[i].uright);
#if defined( EAE6320_PLATFORM_D3D )
		texcoordArray[i].top = Math::cHalf::MakeHalfFromFloat(0.0f);
		texcoordArray[i].bottom = Math::cHalf::MakeHalfFromFloat(1.0f);
#elif defined( EAE6320_PLATFORM_GL )
		texcoordArray[i].top = Math::cHalf::MakeHalfFromFloat(1.0f);
		texcoordArray[i].bottom = Math::cHalf::MakeHalfFromFloat(0.0f);
#endif
	}

		if (!Initialize())
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Font initialization failed");
			goto OnExit;
		}

OnExit:
	if (fontData)
	{
		free(fontData);
	}

	binaryFontData.Free();

	return !wereThereErrors;
}

bool eae6320::Graphics::Font::CleanUp()
{
	bool wereThereErrors = false;
	if(ms_material)
	{
		delete ms_material;
		ms_material = nullptr;
	}
	if(texcoordArray)
	{
		free(texcoordArray);
		texcoordArray = nullptr;
	}
	if (widthArray)
	{
		free(widthArray);
		widthArray = nullptr;
	}
	if(!CleanUpFont())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Font CleanUp failed");
	}
	return !wereThereErrors;
}