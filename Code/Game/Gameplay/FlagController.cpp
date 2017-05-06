#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"
#include "TPSPlayerController.h"
#include "../../Engine/Audio/AudioManager.h"
#include "../../Engine/Audio/AudioClip.h"
#include "../../Engine/Network/NetworkManager.h"
#include "../../Engine/Network/SoundID.h"
#include "../../Engine/UserSettings/UserSettings.h"

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
				if (UserSettings::GetSoundEffectsState())
				{
					Audio::audioClips.at("otherteamflagpicked")->Play();
					Network::NetworkManager::GetSingleton()->TriggerMySoundsOnNetwork2D(Network::SoundID2D::PLAY_MY_TEAM_FLAG_PICKED);
				}
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