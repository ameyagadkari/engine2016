#include "../Graphics/CommonData.h"

namespace
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
	void CommonData::CleanUp()
	{
		if (commonData)
		{
			delete commonData;
		}		
	}

}


