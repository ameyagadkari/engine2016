#include "AudioClip.h"
#include "AudioManager.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

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
	fmodSystem->getChannel(i_channelID, &m_channel);
	const FMOD_RESULT result = fmodSystem->createSound(i_path, i_mode, i_info, &m_clip);
	if (result != FMOD_OK)
	{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to create sound %s: %s", i_path, FMOD_ErrorString(result));
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
bool eae6320::Audio::AudioClip::Play(const bool i_isLooped, const bool i_isPaused)
{
	bool wereThereErrors = false;
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
	const FMOD_RESULT result = fmodSystem->playSound(m_clip, channelGroup, i_isPaused, &m_channel);
	if (result != FMOD_OK)
	{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to play the sound: %s", FMOD_ErrorString(result));
	}
	return !wereThereErrors;
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
