#include "GameObject.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

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
		GameObject* GameObject::Initilaize(const char* relativePath)
		{
			GameObject *gameObject = new GameObject();
			gameObject->mesh = Graphics::Mesh::LoadMesh(relativePath);
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
		void GameObject::SetNewOffset(const float x, const float y)
		{
			offset[0] += x;
			offset[1] += y;
			drawCallBufferData.g_objectPosition_screen[0] = initialOffset[0] + offset[0];
			drawCallBufferData.g_objectPosition_screen[1] = initialOffset[1] + offset[1];
		}
		void GameObject::SetNewInitialOffset(const float x, const float y)
		{
			initialOffset[0] = x;
			initialOffset[1] = y;
		}
	}
}