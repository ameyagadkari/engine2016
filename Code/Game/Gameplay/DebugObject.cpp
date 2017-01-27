#include "DebugObject.h"

#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/Graphics/MeshData.h"

namespace
{
	const char* const materailPath = "data/materials/primitive_material.binmaterial";
}

namespace eae6320
{
	namespace Gameplay
	{
		Graphics::Material* DebugObject::ms_material = nullptr;
		Graphics::Material* DebugObject::GetMaterial()
		{
			return ms_material;
		}
		Graphics::Mesh* DebugObject::GetMesh()const
		{
			return m_mesh;
		}
		Math::cVector DebugObject::GetPosition() const
		{
			return m_position;
		}
		void DebugObject::GetColor(float& i_r, float& i_g, float& i_b)const
		{
			i_r = m_color.r;
			i_g = m_color.g;
			i_b = m_color.b;
		}
		DebugObject::DebugObject(Math::cVector i_position, const float i_r, const float i_g, const float i_b) :
			m_mesh(new Graphics::Mesh()),
			m_position(i_position),
			m_color(i_r, i_g, i_b)
		{
			if (!ms_material)
			{
				ms_material = new Graphics::Material();
				Graphics::Material::LoadMaterial(materailPath, *ms_material);
			}
		}
		DebugObject::~DebugObject()
		{
			if (m_mesh)
			{
				delete m_mesh;
				m_mesh = nullptr;
			}
			if (ms_material)
			{
				delete ms_material;
				ms_material = nullptr;
			}
		}
		void DebugObject::CreateBox(const float i_width, const float i_height, const float i_depth) const
		{
			const float halfWidth = 0.5f*i_width;
			const float halfHeight = 0.5f*i_height;
			const float halfDepth = 0.5f*i_depth;

			const float signs[24][3] =
			{
				// front
				{ -1.0f, -1.0f, -1.0f },
				{ -1.0f, +1.0f, -1.0f },
				{ +1.0f, +1.0f, -1.0f },
				{ +1.0f, -1.0f, -1.0f },
				//back
				{ -1.0f, -1.0f, +1.0f },
				{ +1.0f, -1.0f, +1.0f },
				{ +1.0f, +1.0f, +1.0f },
				{ -1.0f, +1.0f, +1.0f },
				//top
				{ -1.0f, +1.0f, -1.0f },
				{ -1.0f, +1.0f, +1.0f },
				{ +1.0f, +1.0f, +1.0f },
				{ +1.0f, +1.0f, -1.0f },
				//bottom
				{ -1.0f, -1.0f, -1.0f },
				{ +1.0f, -1.0f, -1.0f },
				{ +1.0f, -1.0f, +1.0f },
				{ -1.0f, -1.0f, +1.0f },
				//left
				{ -1.0f, -1.0f, +1.0f },
				{ -1.0f, +1.0f, +1.0f },
				{ -1.0f, +1.0f, -1.0f },
				{ -1.0f, -1.0f, -1.0f },
				//right
				{ +1.0f, -1.0f, -1.0f },
				{ +1.0f, +1.0f, -1.0f },
				{ +1.0f, +1.0f, +1.0f },
				{ +1.0f, -1.0f, +1.0f },
			};

			Graphics::MeshData meshData(16, 24, 36);
			for (size_t i = 0; i < meshData.numberOfVertices; i++)
			{
				meshData.vertexData[i].AddVertexData(signs[i][0] * halfWidth, signs[i][1] * halfHeight, signs[i][2] * halfDepth);
			}

			const uint16_t indices[36] =
			{
				0,1,2,0,2,3,
				4,5,6,4,6,7,
				8,9,10,8,10,11,
				12,13,14,12,14,15,
				16,17,18,16,18,19,
				20,21,22,20,22,23
			};
			for (size_t i = 0; i < meshData.numberOfIndices; i++)
			{
#if defined( EAE6320_PLATFORM_D3D )
				reinterpret_cast<uint16_t*>(meshData.indexData)[meshData.numberOfIndices - 1 - i] = indices[i];
#elif defined( EAE6320_PLATFORM_GL )
				reinterpret_cast<uint16_t*>(meshData.indexData)[i] = indices[i];
#endif
		}

			if (!m_mesh->Initialize(meshData))
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Failed to initialize debug box shape with %f width,%f height,%f depth", i_width, i_height, i_depth);
			}
	}
}
}