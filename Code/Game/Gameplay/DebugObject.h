#ifndef EAE6320_DEBUGOBJECT_H
#define EAE6320_DEBUGOBJECT_H

#include "../../Engine/Math/cVector.h"

namespace eae6320
{
	namespace Graphics
	{
		class Material;
		class Mesh;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class DebugObject
		{
		public:
			static Graphics::Material* GetMaterial();
			Graphics::Mesh* GetMesh()const;
			Math::cVector GetPosition()const;

			DebugObject(Math::cVector i_position = Math::cVector::zero);
			~DebugObject();	
			void CreateBox(const float i_width, const float i_height, const float i_depth, const float i_r, const float i_g, const float i_b) const;

		private:
			static Graphics::Material* ms_material;
			Graphics::Mesh* m_mesh;
			Math::cVector m_position;
		};
	}
}

#endif // !EAE6320_DEBUGOBJECT_H