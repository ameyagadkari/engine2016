#ifndef EAE6320_DEBUGOBJECT_H
#define EAE6320_DEBUGOBJECT_H

#include "../../Engine/Math/cVector.h"
#include "Color.h"
#include <cstdint>

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
	namespace Debug
	{
		namespace Shapes
		{		
			class DebugObject
			{
			public:
				static Graphics::Material* GetMaterial();
				Graphics::Mesh* GetMesh()const;
				Math::cVector GetPosition()const;
				void GetColor(float& i_r, float& i_g, float& i_b)const;

				explicit DebugObject(const Math::cVector i_position = Math::cVector::zero, const Color i_color = { 1.0f,1.0f,1.0f });
				~DebugObject();

				void CreateLine(const Math::cVector i_end) const;
				void CreateBox(const float i_width, const float i_height, const float i_depth) const;
				void CreateSphere(const float i_radius, const uint32_t i_sliceCount, const uint32_t i_stackCount) const;
				void CreateCylinder(const float i_bottomRadius, const float i_topRadius, const float i_height, const uint32_t i_sliceCount, const uint32_t i_stackCount) const;

			private:
				static Graphics::Material* ms_material;
				Graphics::Mesh* m_mesh;
				Math::cVector m_position;
				Color m_color;
			};
		}
	}
}

#endif // !EAE6320_DEBUGOBJECT_H