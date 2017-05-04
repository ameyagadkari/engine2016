#include "AudioClip.h"
#include "AudioManager.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

eae6320::Audio::AudioClip::AudioClip(char const * const i_path, const FMOD_MODE i_mode) :
	m_clip(nullptr)
{
	if(!Initialize(i_path, i_mode))
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

bool eae6320::Audio::AudioClip::Initialize(char const * const i_path, const FMOD_MODE i_mode)
{
	bool wereThereErrors = false;
	FMOD_CREATESOUNDEXINFO *const i_info = nullptr;
	FMOD::System* fmodSystem = AudioManager::GetSingleton()->GetFMODSystem();
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
bool eae6320::Audio::AudioClip::Play(const bool i_isLooped, const bool i_isPaused) const
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
	FMOD::Channel * channel = nullptr;
	FMOD::System* fmodSystem = AudioManager::GetSingleton()->GetFMODSystem();
	fmodSystem->getChannel(0, &channel);
	const FMOD_RESULT result = fmodSystem->playSound(m_clip, channelGroup, i_isPaused, &channel);
	if (result != FMOD_OK)
	{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to play the sound: %s",FMOD_ErrorString(result));
	}
	return !wereThereErrors;
}
