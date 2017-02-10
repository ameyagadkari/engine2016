#include "GameObject2D.h"
#include "../../Engine/Graphics/cSprite.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/Graphics/cTexture.h"
#include "../../Engine/UserSettings/UserSettings.h"

// Helper Function Declarations
//=============================

namespace
{
	void ProcessControlBits(const uint8_t i_controlBits, const eae6320::Gameplay::GameObject2D* i_gameObject2D, eae6320::Gameplay::RectTransform& o_rectTransform);
}

namespace eae6320
{
	namespace Gameplay
	{
		GameObject2D::GameObject2D() :
			sprite(NULL),
			material(new Graphics::Material())
		{}

		GameObject2D::~GameObject2D()
		{
			if (sprite)
			{
				delete sprite;
			}
			if (material)
			{
				delete material;
			}
		}

		GameObject2D * GameObject2D::LoadGameObject2D(const char * const relativePath, const bool doNotUsePixelCoordinatesFromFile, const int16_t x, const int16_t y)
		{
			bool wereThereErrors = false;
			GameObject2D *gameObject2D = new GameObject2D();

			// Load the binary GameObject2D file
			Platform::sDataFromFile binaryGameObject2D;
			{
				std::string errorMessage;
				if (!LoadBinaryFile(relativePath, binaryGameObject2D, &errorMessage))
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, errorMessage.c_str());
					Logging::OutputError("Failed to load the gameobject2d mesh file \"%s\": %s", relativePath, errorMessage.c_str());
					goto OnExit;
				}
			}


			// Casting data to uint8_t* for pointer arithematic
			uint8_t* data = reinterpret_cast<uint8_t*>(binaryGameObject2D.data);

			uint8_t offsetToAdd = 0;
			uint8_t controlBits = 0;

			// Extracting Binary Data
			{
				// Extracting the texture coordinates
				const Graphics::Sprite::TextureCoordinates textureCoordinates = *reinterpret_cast<Graphics::Sprite::TextureCoordinates*>(data);

				// Extracting the pixel coordinates
				data += sizeof(Graphics::Sprite::TextureCoordinates);
				{
					gameObject2D->rectTransform = *reinterpret_cast<RectTransform*>(data);
				}
				// Extracting the anchor
				data += sizeof(RectTransform);
				{
					controlBits = *reinterpret_cast<uint8_t*>(data);
				}
				// Extracting the anchor
				data += sizeof(uint8_t);
				{
					gameObject2D->anchor = static_cast<Anchor>(*reinterpret_cast<uint8_t*>(data));
				}
				// Extracting Offset To Add
				data += sizeof(uint8_t);
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Material File Path and loading material
				data += sizeof(offsetToAdd);
				{
					const char * const materialPath = reinterpret_cast<const char *>(data);
					if (!Graphics::Material::LoadMaterial(materialPath, *gameObject2D->material))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary effect file: %s", materialPath);
						goto OnExit;
					}
				}
				if (doNotUsePixelCoordinatesFromFile)
				{
					gameObject2D->rectTransform.pixelCoordinates.x = x;
					gameObject2D->rectTransform.pixelCoordinates.y = y;
				}
				// Creating new Sprite
				{
					ProcessControlBits(controlBits, gameObject2D, gameObject2D->rectTransform);
					const RectTransform rectTranform = gameObject2D->rectTransform;
					const Anchor anchor = gameObject2D->anchor;
					const Graphics::Sprite::ScreenPosition screenPosition = ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(rectTranform, anchor);
					gameObject2D->sprite = new Graphics::cSprite(screenPosition, textureCoordinates);
				}
			}

		OnExit:

			binaryGameObject2D.Free();

			if (wereThereErrors)
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Gameobject2D in %s was not loaded", relativePath);
				return NULL;
			}
			else
			{
				return gameObject2D;
			}
		}

#pragma region Gets
		Graphics::Material * GameObject2D::GetMaterial() const
		{
			return material;
		}
		Graphics::cSprite * GameObject2D::GetSprite() const
		{
			return sprite;
		}
		RectTransform GameObject2D::GetRectTransform() const
		{
			return rectTransform;
		}
		Anchor GameObject2D::GetAnchor() const
		{
			return anchor;
		}
#pragma endregion

#pragma region Sets
		void GameObject2D::SetMaterial(Graphics::Material * const material)
		{
			this->material = material;
		}
		void GameObject2D::SetSprite(Graphics::cSprite * const sprite)
		{
			this->sprite = sprite;
		}
		void GameObject2D::SetRectTransform(const RectTransform rectTransform)
		{
			this->rectTransform = rectTransform;
		}
		void GameObject2D::SetAnchor(const Anchor anchor)
		{
			this->anchor = anchor;
		}
#pragma endregion
	}
}

namespace
{
	void ProcessControlBits(const uint8_t i_controlBits, const eae6320::Gameplay::GameObject2D* i_gameObject2D, eae6320::Gameplay::RectTransform& o_rectTransform)
	{
		const uint16_t textureWidth = i_gameObject2D->GetMaterial()->GetTexture()->GetWidth();
		const uint16_t textureHeight = i_gameObject2D->GetMaterial()->GetTexture()->GetHeight();

		const float aspectRatioTexture = static_cast<float>(textureWidth) / textureHeight;
		switch (i_controlBits)
		{
			//maintain width
		case 0x80:
			o_rectTransform.height = static_cast<uint16_t>(o_rectTransform.width / aspectRatioTexture);
			break;
			//maintain height
		case 0x40:
			o_rectTransform.width = static_cast<uint16_t>(o_rectTransform.height * aspectRatioTexture);
			break;
			//maintain both
		case 0xC0:
			break;
		default:
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Wrong control bits. You need to maintain either width or height of sprite. This should not happen");
			break;
		}
	}
}
eae6320::Graphics::Sprite::ScreenPosition eae6320::Gameplay::ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(const RectTransform i_rectTransform, const Anchor i_anchor)
{
	Graphics::Sprite::ScreenPosition returnValue;

	const float widthMultiplier = 2.0f / UserSettings::GetResolutionWidth();
	const float heightMultiplier = 2.0f / UserSettings::GetResolutionHeight();

	switch (i_anchor)
	{
	case Anchor::TOP_LEFT:
		returnValue.left = i_rectTransform.pixelCoordinates.x*widthMultiplier;
		returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		break;
	case Anchor::BOTTOM_LEFT:
		returnValue.left = i_rectTransform.pixelCoordinates.x*widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		break;
	case Anchor::TOP_RIGHT:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		returnValue.right = i_rectTransform.pixelCoordinates.x*widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		break;
	case Anchor::BOTTOM_RIGHT:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		returnValue.right = i_rectTransform.pixelCoordinates.x*widthMultiplier;
		returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		break;
	case Anchor::CENTER:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		break;
	case Anchor::LEFT_CENTER:
		returnValue.left = i_rectTransform.pixelCoordinates.x *widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		break;
	case Anchor::BOTTOM_CENTER:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		break;
	case Anchor::TOP_CENTER:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier;
		returnValue.right = (i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*heightMultiplier;
		break;
	case Anchor::RIGHT_CENTER:
		returnValue.left = (i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*widthMultiplier;
		returnValue.top = (i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		returnValue.right = i_rectTransform.pixelCoordinates.x *widthMultiplier;
		returnValue.bottom = (i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*heightMultiplier;
		break;
	default:
		break;
	}
	return returnValue;
}
