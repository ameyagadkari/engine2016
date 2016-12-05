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
	eae6320::Graphics::Sprite::ScreenPosition ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(const eae6320::Gameplay::RectTransform i_rectTransform, const eae6320::Gameplay::Anchor i_anchor, const  uint16_t i_textureWidth, const uint16_t i_textureHeight);
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

		GameObject2D * GameObject2D::LoadGameObject2D(const char * const relativePath)
		{
			bool wereThereErrors = false;
			GameObject2D *gameObject2D = new GameObject2D();

			// Load the binary GameObject2D file
			eae6320::Platform::sDataFromFile binaryGameObject2D;
			{
				std::string errorMessage;
				if (!eae6320::Platform::LoadBinaryFile(relativePath, binaryGameObject2D, &errorMessage))
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, errorMessage.c_str());
					eae6320::Logging::OutputError("Failed to load the gameobject2d mesh file \"%s\": %s", relativePath, errorMessage.c_str());
					goto OnExit;
				}
			}


			// Casting data to uint8_t* for pointer arithematic
			uint8_t* data = reinterpret_cast<uint8_t*>(binaryGameObject2D.data);

			uint8_t offsetToAdd = 0;

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

				// Creating new Sprite
				{

					const RectTransform rectTranform = gameObject2D->rectTransform;
					const Anchor anchor = gameObject2D->anchor;
					const uint16_t textureWidth = gameObject2D->material->GetTexture()->GetWidth();
					const uint16_t textureHeight = gameObject2D->material->GetTexture()->GetHeight();
					const Graphics::Sprite::ScreenPosition screenPosition = ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(rectTranform, anchor, textureWidth, textureHeight);
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
#pragma endregion
	}
}

namespace
{
	eae6320::Graphics::Sprite::ScreenPosition ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(const eae6320::Gameplay::RectTransform i_rectTransform, const eae6320::Gameplay::Anchor i_anchor, const uint16_t i_textureWidth, const uint16_t i_textureHeight)
	{
		eae6320::Graphics::Sprite::ScreenPosition returnValue;

		const float screenAspectRatio = static_cast<float>(eae6320::UserSettings::GetResolutionWidth()) / eae6320::UserSettings::GetResolutionHeight();
		const float textureAspectRatio = static_cast<float>(i_textureWidth) / i_textureHeight;

		const float widthMultiplier = 2.0f / eae6320::UserSettings::GetResolutionWidth();// *screenAspectRatio;
		const float heightMultiplier = 2.0f / eae6320::UserSettings::GetResolutionHeight();// *screenAspectRatio;

		switch (i_anchor)
		{
		case eae6320::Gameplay::Anchor::TOP_LEFT:
			returnValue.left = i_rectTransform.pixelCoordinates.x*widthMultiplier;
			returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier;
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::BOTTOM_LEFT:
			returnValue.left = i_rectTransform.pixelCoordinates.x*widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::TOP_RIGHT:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier;
			returnValue.right = i_rectTransform.pixelCoordinates.x*widthMultiplier;
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::BOTTOM_RIGHT:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			returnValue.right = i_rectTransform.pixelCoordinates.x*widthMultiplier;
			returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::CENTER:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::LEFT_CENTER:
			returnValue.left = i_rectTransform.pixelCoordinates.x *widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::BOTTOM_CENTER:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = i_rectTransform.pixelCoordinates.y*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::TOP_CENTER:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.top = i_rectTransform.pixelCoordinates.y*heightMultiplier; 
			returnValue.right = ((i_rectTransform.pixelCoordinates.x + static_cast<int16_t>(i_rectTransform.width / 2))*textureAspectRatio)*widthMultiplier;
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height))*textureAspectRatio)*heightMultiplier;
			break;
		case eae6320::Gameplay::Anchor::RIGHT_CENTER:
			returnValue.left = ((i_rectTransform.pixelCoordinates.x - static_cast<int16_t>(i_rectTransform.width))*textureAspectRatio)*widthMultiplier;
			returnValue.top = ((i_rectTransform.pixelCoordinates.y + static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			returnValue.right = i_rectTransform.pixelCoordinates.x *widthMultiplier; 
			returnValue.bottom = ((i_rectTransform.pixelCoordinates.y - static_cast<int16_t>(i_rectTransform.height / 2))*textureAspectRatio)*heightMultiplier;
			break;
		default:
			break;
		}
		return returnValue;
	}
}