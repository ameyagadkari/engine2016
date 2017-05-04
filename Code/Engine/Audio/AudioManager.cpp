#include "AudioManager.h"
#include "AudioClip.h"
#include "../UserSettings/UserSettings.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"

namespace
{
	char const * const musicPath = "data/audio/backgroundmusic.wav";
}

eae6320::Audio::AudioManager* eae6320::Audio::AudioManager::singleton = nullptr;

eae6320::Audio::AudioManager::AudioManager() :
	m_backgroundMusic(nullptr),
	m_fmodSystem(nullptr),
	m_isAudioEnabled(false)
{}

bool eae6320::Audio::AudioManager::Initialize()
{
	bool wereThereErrors = false;
	singleton = new AudioManager();
	if (UserSettings::GetMusicState() || UserSettings::GetSoundEffectsState())
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
			const FMOD_INITFLAGS flags = FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED;
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
	if (UserSettings::GetMusicState())
	{
		// Init background music
		{
			const FMOD_MODE mode = FMOD_2D;
			singleton->m_backgroundMusic = new AudioClip(musicPath, mode);
		}
		// Start background music
		{
			const bool isLooped = true;
			singleton->m_backgroundMusic->Play(isLooped);
		}
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
	if (singleton)
	{
		if (singleton->m_isAudioEnabled)
		{
			if (UserSettings::GetMusicState())
			{
				// Releases background music
				if (singleton->m_backgroundMusic)
				{
					delete singleton->m_backgroundMusic;
					singleton->m_backgroundMusic = nullptr;
				}
			}
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
		delete singleton;
		singleton = nullptr;
	}
	return !wereThereErrors;
}

FMOD::System* eae6320::Audio::AudioManager::GetFMODSystem() const
{
	return m_fmodSystem;
}