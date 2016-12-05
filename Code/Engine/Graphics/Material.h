#ifndef EAE6320_MATERIAL_H
#define EAE6320_MATERIAL_H

#include <cstdint>

// Forward Declarations
//=====================
namespace eae6320
{
	namespace Graphics
	{
		class ConstantBuffer;
		class Effect;
		class cTexture;
	}
}

namespace eae6320
{
	namespace Graphics
	{
		class Material
		{
		public:
			Material();
			~Material();

			static bool LoadMaterial(const char * const i_relativePath, Material& o_material);

			bool CleanUpMaterial();
			void BindMaterial()const;

			uint32_t GetMaterialUUID()const;
			Effect * GetEffect()const;
			cTexture* GetTexture()const;

		private:
			static uint32_t s_previousEffectUUID;
			uint32_t m_materialUUID;
			ConstantBuffer *m_materialBuffer;
			Effect *m_effect;
			cTexture *m_texture;
		};
	}
}

#endif