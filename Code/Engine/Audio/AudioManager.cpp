#include "AudioManager.h"
#include "../UserSettings/UserSettings.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"

eae6320::Audio::AudioManager::AudioManager():
	m_fmodSystem(nullptr),
	m_isAudioEnabled(false)
{}

bool eae6320::Audio::AudioManager::Initialize()
{
	bool wereThereErrors = false;
	singleton = new AudioManager();
	if(UserSettings::GetMusicState() || UserSettings::GetSoundEffectsState())
	{
		singleton->m_isAudioEnabled = true;

		// Create FMOD System
		{
			const FMOD_RESULT result = System_Create(&singleton->m_fmodSystem);
			if (result != FMOD_OK)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, FMOD_ErrorString(result));
				Logging::OutputError("Failed to create FMOD system %s", FMOD_ErrorString(result));
				goto OnExit;
			}
		}

		// Initialize FMOD System
		{
			const int maxchannels = 32;
			const FMOD_INITFLAGS flags = FMOD_INIT_NORMAL;
			void * const extradriverdata = nullptr;
			const FMOD_RESULT result = singleton->m_fmodSystem->init(maxchannels, flags, extradriverdata);
			if (result != FMOD_OK)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, FMOD_ErrorString(result));
				Logging::OutputError("Failed to initialize FMOD system %s", FMOD_ErrorString(result));
				goto OnExit;
			}
		}
	}
	else
	{
		singleton->m_isAudioEnabled = false;
	}

OnExit:
	return !wereThereErrors;
}

eae6320::Audio::AudioManager * eae6320::Audio::AudioManager::GetSingleton()
{
	if (singleton)return singleton;
	EAE6320_ASSERTF(false, "Initialize Audio Manager");
	Logging::OutputError("Audio Manager not initialized");
	return nullptr;
}


bool eae6320::Audio::AudioManager::CleanUp()
{
	bool wereThereErrors = false;
	if (singleton->m_isAudioEnabled)
	{
		// Closes and release FMOD system
		{
			const FMOD_RESULT result = singleton->m_fmodSystem->release();
			if (result != FMOD_OK)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, FMOD_ErrorString(result));
				Logging::OutputError("Failed to close and release FMOD system %s", FMOD_ErrorString(result));
			}
		}
	}
	if (singleton)
	{
		delete singleton;
		singleton = nullptr;
	}
	return !wereThereErrors;
}

FMOD::System* eae6320::Audio::AudioManager::GetFMODSystem() const
{
	return m_fmodSystem;
}