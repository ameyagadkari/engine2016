#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"
#include "../../Engine/Physics/Physics.h"
#include "../../Engine/Physics/HitData.h"

uint32_t const eae6320::Gameplay::FlagController::classUUID = StringHandler::HashedString("FlagController").GetHash();

void eae6320::Gameplay::FlagController::UpdatePosition(Transform & io_transform)
{
	/*if (m_numberOfFlagTriangles && m_playerTransform)
	{
		Physics::HitData hitData;
		if (m_flagNotCarrying)
		{
			CheckCollisionForFlag(m_playerTransform->m_position,
				m_playerTransform->m_position + m_playerTransform->m_localAxes.m_forward*100.0f,
				m_numberOfFlagTriangles,
				m_flagTriangles,
				hitData);
			if (Physics::hasIntersected)
			{
				m_flagNotCarrying = false;
			}
		}
		else
		{
			
		}*/
	if (m_playerTransform)io_transform.m_position = m_playerTransform->m_position;
	//}
	
}

void eae6320::Gameplay::FlagController::UpdateOrientation(Transform & io_transform)
{
	if (m_playerTransform)io_transform.SetOrientationEular(m_playerTransform->GetOrientationEular());
	io_transform.UpdateLocalAxes();
}