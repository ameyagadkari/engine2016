#include "CommonData.h"

namespace eae6320
{
	namespace Graphics
	{
		CommonData* CommonData::commonData = nullptr;

		inline CommonData::CommonData() 
		{
#if defined( EAE6320_PLATFORM_D3D )
			compiledVertexShader = reinterpret_cast<eae6320::Platform::sDataFromFile*>(malloc(sizeof(eae6320::Platform::sDataFromFile)));
#endif
		}

		inline CommonData::~CommonData() 
		{
#if defined( EAE6320_PLATFORM_D3D )
			if (compiledVertexShader)
			{
				free(compiledVertexShader);
				compiledVertexShader = NULL;
			}
#endif
		}

		CommonData * CommonData::GetCommonData()
		{
			if (!commonData)
			{
				commonData = new CommonData();
			}
			return commonData;
		}
		bool eae6320::Graphics::CommonData::Initialize()
		{
			if (!commonData)
			{
				commonData = new CommonData();
			}
			if (commonData)
			{
				return true;
			}
			return false;
		}
		bool CommonData::CleanUp()
		{
			if (commonData)
			{
				delete commonData;
				return true;
			}
			return false;
		}
	}
}


