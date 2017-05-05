#include "ScoreZoneController.h"
#include "TPSPlayerController.h"
#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"
#include "../../Engine/Audio/AudioManager.h"
#include "../../Engine/Audio/AudioClip.h"
#include "../../Engine/Network/NetworkManager.h"
#include "../../Engine/Network/SoundID.h"
#include "../../Engine/UserSettings/UserSettings.h"

uint32_t const eae6320::Gameplay::ScoreZoneController::classUUID(StringHandler::HashedString("ScoreZoneController").GetHash());

void eae6320::Gameplay::ScoreZoneController::UpdatePosition(Transform & io_transform)
{
	if (m_playerTransform)
	{
		float distance = m_playerTransform->m_position.DistanceBetween(io_transform.m_position);
		if (distance < 150.0f)
		{
			if (m_tpsPlayerController->GetCarryFlag())
			{
				if(UserSettings::GetSoundEffectsState())
				{
					Audio::audioClips.at("myteamscored")->Play();
					Network::NetworkManager::GetSingleton()->TriggerMySoundsOnNetwork(Network::SoundID::PLAY_OTHER_TEAM_SCORED);
				}
				m_score++;
				m_tpsPlayerController->SetCarryFlag(false);
				m_flagController->ResetFlagPostion();
			}
		}
	}
}

void eae6320::Gameplay::ScoreZoneController::UpdateOrientation(Transform & io_transform)
{}