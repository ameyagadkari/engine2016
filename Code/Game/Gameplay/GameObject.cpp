#include "GameObject.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"

namespace eae6320
{
	namespace Gameplay
	{
		inline GameObject::GameObject() 
		{
			initialOffset[0] = 0.0f;
			initialOffset[1] = 0.0f;
			offset[0] = 0.0f;
			offset[1] = 0.0f;

		}
		inline GameObject::~GameObject() 
		{
			if (mesh && !mesh->CleanUp())
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Mesh cleanup failed");
			}
			if (mesh)
				delete mesh;
		}

#pragma region Gets
		bool GameObject::GetIsStatic()const
		{
			return isStatic;
		}
		Graphics::Mesh * GameObject::GetMesh() const
		{
			return mesh;
		}
		Graphics::ConstantBufferData::sDrawCall GameObject::GetDrawCallBufferData()const
		{
			return drawCallBufferData;
		}
#pragma endregion
			
#pragma region Sets
		void GameObject::SetIsStatic(const bool isStatic)
		{
			this->isStatic = isStatic;
		}
		void GameObject::SetMesh(Graphics::Mesh * const mesh)
		{
			this->mesh = mesh;
		}
		void GameObject::SetDrawCallBufferData(const Graphics::ConstantBufferData::sDrawCall drawCallBufferData)
		{
			this->drawCallBufferData = drawCallBufferData;
		}
#pragma endregion


		void GameObject::UpdateGameObjectPosition()
		{
			float localOffset[2] = { 0.0f,0.0f };
			if (!isStatic)
			{
				if (UserInput::IsKeyPressed(VK_UP) || UserInput::IsKeyPressed(0x57))//W
					localOffset[1] += 1.0f;
				if (UserInput::IsKeyPressed(VK_DOWN) || UserInput::IsKeyPressed(0x53))//S
					localOffset[1] -= 1.0f;
				if (UserInput::IsKeyPressed(VK_LEFT) || UserInput::IsKeyPressed(0x41))//A
					localOffset[0] -= 1.0f;
				if (UserInput::IsKeyPressed(VK_RIGHT) || UserInput::IsKeyPressed(0x44))//D
					localOffset[0] += 1.0f;

				const float speed_unitsPerSecond = 0.5f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset[0] *= offsetModifier;
				localOffset[1] *= offsetModifier;
			}	
			offset[0] += localOffset[0];
			offset[1] += localOffset[1];
			drawCallBufferData.g_objectPosition_screen[0] = initialOffset[0] + offset[0];
			drawCallBufferData.g_objectPosition_screen[1] = initialOffset[1] + offset[1];
		}
		GameObject* GameObject::Initilaize(const char* relativePath)
		{
			GameObject *gameObject = new GameObject();
			gameObject->mesh = Graphics::Mesh::LoadMesh(relativePath);
			gameObject->isStatic = true;
			gameObject->drawCallBufferData.g_objectPosition_screen[0] = gameObject->initialOffset[0];
			gameObject->drawCallBufferData.g_objectPosition_screen[1] = gameObject->initialOffset[1];
			if (gameObject->mesh)
			{
				return gameObject;
			}
			EAE6320_ASSERT(false);
			Logging::OutputError("Mesh at %s was not loaded", relativePath);
			return NULL;
		}
		void GameObject::SetNewInitialOffset(const float x, const float y)
		{
			initialOffset[0] = x;
			initialOffset[1] = y;
		}
	}
}