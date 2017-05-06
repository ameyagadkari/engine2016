#include "../../Engine/Audio/ConfigurationAudio.h"

#include "ScoreZoneController.h"
#include "TPSPlayerController.h"
#include "FlagController.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"
#include "../../Engine/UserSettings/UserSettings.h"

#if PLAYMYPLAYERSOUNDS
#include "../../Engine/Audio/AudioManager.h"
#include "../../Engine/Audio/AudioClip.h"
#else
#include "../../Engine/Network/NetworkManager.h"
#include "../../Engine/Network/SoundID.h"
#include "../../Engine/Network/NetworkScene.h"
#endif

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
#if PLAYMYPLAYERSOUNDS
					Audio::audioClips.at("myteamscored")->Play();
#else
					if (Network::NetworkScene::currentGameState == Network::NetworkScene::RunMultiplayer)
					{
						Network::NetworkManager::GetSingleton()->TriggerMySoundsOnNetwork2D(Network::SoundID2D::PLAY_OTHER_TEAM_SCORED);
					}
#endif
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