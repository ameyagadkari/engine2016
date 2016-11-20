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

			static bool LoadMaterial(const char * const relativePath, Material& o_material);

			bool CleanUpMaterial();
			void BindMaterial();

			uint32_t GetMaterialUUID()const;

		private:
			static uint32_t previousEffectUUID;
			uint32_t materialUUID = 0;
			ConstantBuffer *materialBuffer;
			Effect *effect;
		};
	}
}

#endif