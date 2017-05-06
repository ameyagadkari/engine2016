#include "AudioClip.h"
#include "AudioManager.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

namespace
{
	void ConvertFromcVectorToFMOD_Vector(const eae6320::Math::cVector i_input, FMOD_VECTOR& o_output);
	const float distanceFactor = 3779.527f;
}

eae6320::Audio::AudioClip::AudioClip(char const * const i_path, const FMOD_MODE i_mode, const int i_channelID) :
	m_clip(nullptr),
	m_channel(nullptr)
{
	if (!Initialize(i_path, i_mode, i_channelID))
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Failed to initialize Audio Clip: %s", i_path);
	}
}

eae6320::Audio::AudioClip::~AudioClip()
{
	if (!CleanUp())
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Failed to clean up Audio Clip");
	}
}

bool eae6320::Audio::AudioClip::Initialize(char const * const i_path, const FMOD_MODE i_mode, const int i_channelID)
{
	bool wereThereErrors = false;
	FMOD_CREATESOUNDEXINFO *const i_info = nullptr;
	FMOD::System* fmodSystem = AudioManager::GetSingleton()->GetFMODSystem();
	{
		const FMOD_RESULT result = fmodSystem->getChannel(i_channelID, &m_channel);
		if (result != FMOD_OK)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, FMOD_ErrorString(result));
			Logging::OutputError("Failed to get the channel specified by id:%d %s", i_channelID, FMOD_ErrorString(result));
		}

	}
	{
		const FMOD_RESULT result = fmodSystem->createSound(i_path, i_mode, i_info, &m_clip);
		if (result != FMOD_OK)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, FMOD_ErrorString(result));
			Logging::OutputError("Failed to create sound %s: %s", i_path, FMOD_ErrorString(result));
		}
	}
	if (i_mode == FMOD_3D)
	{
		const FMOD_RESULT result = m_clip->set3DMinMaxDistance(0.05f * distanceFactor, 100.0f * distanceFactor);
		if (result != FMOD_OK)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, FMOD_ErrorString(result));
			Logging::OutputError("Failed to set min-max distance for sound %s: %s", i_path, FMOD_ErrorString(result));
		}
	}
	return !wereThereErrors;
}

bool eae6320::Audio::AudioClip::CleanUp() const
{
	bool wereThereErrors = false;
	const FMOD_RESULT result = m_clip->release();
	if (result != FMOD_OK)
	{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to release sound %s", FMOD_ErrorString(result));
	}
	return !wereThereErrors;
}
void eae6320::Audio::AudioClip::Play(const bool i_isLooped, const bool i_isPaused)
{
	if (!i_isLooped)
	{
		m_clip->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		m_clip->setMode(FMOD_LOOP_NORMAL);
		m_clip->setLoopCount(-1);
	}
	FMOD::ChannelGroup *const channelGroup = nullptr;
	FMOD::System* fmodSystem = AudioManager::GetSingleton()->GetFMODSystem();
	if (isWindowInFocus)
	{
		const FMOD_RESULT result = fmodSystem->playSound(m_clip, channelGroup, i_isPaused, &m_channel);
		if (result != FMOD_OK)
		{
			EAE6320_ASSERTF(false, FMOD_ErrorString(result));
			Logging::OutputError("Failed to play the sound: %s", FMOD_ErrorString(result));
		}
	}
}

void eae6320::Audio::AudioClip::Play3D(const bool i_isLooped, const bool i_isPaused, const Math::cVector i_position, const Math::cVector i_velocity)
{
	if(!GetIsPlaying())Play(i_isLooped, i_isPaused);
	FMOD_VECTOR position;
	FMOD_VECTOR velocity;
	ConvertFromcVectorToFMOD_Vector(i_position, position);
	ConvertFromcVectorToFMOD_Vector(i_velocity, velocity);
	const FMOD_RESULT result = m_channel->set3DAttributes(&position, &velocity);
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to set 3D Attributes for the channel: %s", FMOD_ErrorString(result));
	}
}

void eae6320::Audio::AudioClip::SetVolume(const float i_value) const
{
	const FMOD_RESULT result = m_channel->setVolume(i_value);
	if (result != FMOD_OK)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to set the volume: %s", FMOD_ErrorString(result));
	}
}

float eae6320::Audio::AudioClip::GetVolume() const
{
	float returnValue;
	const FMOD_RESULT result = m_channel->getVolume(&returnValue);
	if (result != FMOD_OK)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to get the volume: %s", FMOD_ErrorString(result));
	}
	return returnValue;
}

bool eae6320::Audio::AudioClip::GetIsPlaying() const
{
	bool returnValue;
	const FMOD_RESULT result = m_channel->isPlaying(&returnValue);
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to get isplaying status: %s", FMOD_ErrorString(result));
	}
	return returnValue;
}

bool eae6320::Audio::AudioClip::GetPaused() const
{
	bool returnValue;
	const FMOD_RESULT result = m_channel->getPaused(&returnValue);
	if (result != FMOD_OK)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to get pause status: %s", FMOD_ErrorString(result));
	}
	return returnValue;
}


void eae6320::Audio::AudioClip::SetPaused(const bool i_value) const
{
	const FMOD_RESULT result = m_channel->setPaused(i_value);
	if (result != FMOD_OK)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to %s: %s", i_value ? "Pause" : "UnPause", FMOD_ErrorString(result));
	}
}

void eae6320::Audio::AudioClip::Stop() const
{
	const FMOD_RESULT result = m_channel->stop();
	if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE && result != FMOD_ERR_CHANNEL_STOLEN)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to stop playing: %s", FMOD_ErrorString(result));
	}

}

namespace
{
	void ConvertFromcVectorToFMOD_Vector(const eae6320::Math::cVector i_input, FMOD_VECTOR & o_output)
	{
		o_output.x = i_input.x;
		o_output.y = i_input.y;
		o_output.z = i_input.z;
	}
}