#include "GameObject2D.h"
#include "../../Engine/Graphics/cSprite.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Platform/Platform.h"


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
				Graphics::Sprite::ScreenPosition screenPosition;
				Graphics::Sprite::TextureCoordinates textureCoordinates;
				// Extracting the screen position 
				{
					screenPosition = *reinterpret_cast<Graphics::Sprite::ScreenPosition *>(data);
				}
				// Extracting the texture coordinates
				data += sizeof(Graphics::Sprite::ScreenPosition);
				{
					textureCoordinates = *reinterpret_cast<Graphics::Sprite::TextureCoordinates *>(data);
				}
				// Creating new Sprite
				gameObject2D->sprite = new Graphics::cSprite(screenPosition, textureCoordinates);
				// Extracting Offset To Add
				data += sizeof(Graphics::Sprite::TextureCoordinates);
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