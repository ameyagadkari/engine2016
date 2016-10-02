#include "CommonData.h"

namespace eae6320
{
	namespace Graphics
	{
		CommonData* CommonData::commonData = nullptr;

		inline CommonData::CommonData() {}

		inline CommonData::~CommonData() {}

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


