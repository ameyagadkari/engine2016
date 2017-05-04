#include "AudioManager.h"
#include "../UserSettings/UserSettings.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"

eae6320::Audio::AudioManager::AudioManager():
	fmodSystem(nullptr),
	isAudioEnabled(false)
{}

bool eae6320::Audio::AudioManager::Initialize()
{
	bool wereThereErrors = false;
	singleton = new AudioManager();
	if(UserSettings::GetMusicState() || UserSettings::GetSoundEffectsState())
	{
		singleton->isAudioEnabled = true;

		// Create FMOD System
		{
			const FMOD_RESULT result = System_Create(&singleton->fmodSystem);
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
			const FMOD_RESULT result = singleton->fmodSystem->init(maxchannels, flags, extradriverdata);
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
		singleton->isAudioEnabled = false;
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
	if (singleton->isAudioEnabled)
	{
		// Closes and release FMOD system
		{
			const FMOD_RESULT result = singleton->fmodSystem->release();
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
