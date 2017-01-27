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
		struct Color
		{
			float r, g, b;
			Color(float i_r, float i_g, float i_b) :
				r(i_r), g(i_g), b(i_b)
			{}
		};
		class DebugObject
		{
		public:
			static Graphics::Material* GetMaterial();
			Graphics::Mesh* GetMesh()const;
			Math::cVector GetPosition()const;
			void GetColor(float& i_r, float& i_g, float& i_b)const;

			DebugObject(Math::cVector i_position = Math::cVector::zero, const float i_r = 1.0f, const float i_g = 1.0f, const float i_b = 1.0f);
			~DebugObject();
			void CreateBox(const float i_width, const float i_height, const float i_depth) const;

		private:
			static Graphics::Material* ms_material;
			Graphics::Mesh* m_mesh;
			Math::cVector m_position;
			Color m_color;
		};
	}
}

#endif // !EAE6320_DEBUGOBJECT_H