#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"
#include "TPSPlayerController.h"

uint32_t const eae6320::Gameplay::FlagController::classUUID = StringHandler::HashedString("FlagController").GetHash();

void eae6320::Gameplay::FlagController::UpdatePosition(Transform & io_transform)
{
	if (m_resetFlagPosition)
	{
		io_transform.m_position = m_resetPosition;
		m_resetFlagPosition = false;
	}
	if (m_playerTransform)
	{
		if (m_flagNotCarrying)
		{
			float distance = m_playerTransform->m_position.DistanceBetween(io_transform.m_position);
			if (distance < 150.0f) 
			{
				m_resetPosition = io_transform.m_position;
				m_flagNotCarrying = false;
				m_tpsPlayerController->SetCarryFlag(true);
			}
		}
		else
		{
			io_transform.m_position = m_playerTransform->m_position;
		}
	}
}

void eae6320::Gameplay::FlagController::UpdateOrientation(Transform & io_transform)
{}