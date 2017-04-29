#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"

uint32_t const eae6320::Gameplay::FlagController::classUUID = StringHandler::HashedString("FlagController").GetHash();

void eae6320::Gameplay::FlagController::UpdatePosition(Transform & io_transform)
{
	if (m_playerTransform)
	{
		if (m_flagNotCarrying)
		{
			float dist = m_playerTransform->m_position.DistanceBetween(io_transform.m_position);
			if (dist < 150.0f)m_flagNotCarrying = false;
		}
		else
		{
			io_transform.m_position = m_playerTransform->m_position;
		}
	}
}

void eae6320::Gameplay::FlagController::UpdateOrientation(Transform & io_transform)
{}