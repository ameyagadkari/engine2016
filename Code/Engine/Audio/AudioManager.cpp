#include "AudioManager.h"
#include "AudioClip.h"
#include "../UserSettings/UserSettings.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"
#include "../Windows/Includes.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include <regex>


namespace
{
	void GenerateRelativePaths(std::string prefix);
	std::vector<std::string> relativePaths;
	std::vector<std::string> fileNames;
	const std::regex pattern_match("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");
	const float speed_unitsPerSecond = 0.75f;
}

std::map<const std::string, eae6320::Audio::AudioClip*> eae6320::Audio::audioClips;
eae6320::Audio::AudioManager* eae6320::Audio::AudioManager::singleton = nullptr;

eae6320::Audio::AudioManager::AudioManager(const int i_maxchannels) :
	m_maxchannels(i_maxchannels),
	m_fmodSystem(nullptr),
	m_isAudioEnabled(false)
{}

bool eae6320::Audio::AudioManager::Initialize(const int i_maxchannels)
{
	bool wereThereErrors = false;
	singleton = new AudioManager(i_maxchannels);
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
			const FMOD_INITFLAGS flags = FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED;
			void * const extradriverdata = nullptr;
			const FMOD_RESULT result = singleton->m_fmodSystem->init(i_maxchannels, flags, extradriverdata);
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

	int channelID = 0;
	// Loading 2D sounds
	{
		GenerateRelativePaths("data/audio/2D/");
		const size_t numberOfAudioFiles = relativePaths.size();
		for (size_t i = 0; i < numberOfAudioFiles; i++, channelID++)
		{
			EAE6320_ASSERTF(channelID < i_maxchannels, "Increase number of channels");
			const FMOD_MODE mode = FMOD_2D;
			audioClips[fileNames[i]] = new AudioClip(relativePaths[i].c_str(), mode, channelID);
		}
	}

	// Loading 3D sounds
	{
		GenerateRelativePaths("data/audio/3D/");
		const size_t numberOfAudioFiles = relativePaths.size();
		for (size_t i = 0; i < numberOfAudioFiles; i++, channelID++)
		{
			EAE6320_ASSERTF(channelID < i_maxchannels, "Increase number of channels");
			const FMOD_MODE mode = FMOD_3D;
			audioClips[fileNames[i]] = new AudioClip(relativePaths[i].c_str(), mode, channelID);
		}
	}

	if (UserSettings::GetMusicState())
	{
		// Start background music
		const bool isLooped = true;
		audioClips.at("backgroundmusic")->Play(isLooped);
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
			// Deleting all sounds
			{
				for (auto& audioClip : audioClips)
				{
					if (audioClip.second)
					{
						delete audioClip.second;
						audioClip.second = nullptr;
					}
				}
				audioClips.clear();
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

namespace
{
	void GenerateRelativePaths(std::string prefix)
	{
		relativePaths.clear();
		fileNames.clear();
		WIN32_FIND_DATA search_data;
		memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
		HANDLE handle = FindFirstFile((prefix + "*").c_str(), &search_data);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!strcmp(search_data.cFileName, "."))continue;
				if (!strcmp(search_data.cFileName, ".."))continue;
				relativePaths.push_back((prefix + search_data.cFileName).c_str());
				fileNames.push_back(regex_replace(static_cast<std::string>(search_data.cFileName), pattern_match, pattern_replace));
			} while (FindNextFile(handle, &search_data));
		}
		FindClose(handle);
	}
}

float eae6320::Audio::ChangeMusicVolume(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue)
{
	float returnValue = i_currentValue;
	float offsetModifier;
	if (UserInput::GetKey(VK_LEFT))
	{
		offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		returnValue = i_currentValue > i_minValue ? i_currentValue - offsetModifier : i_minValue;
	}
	else if (UserInput::GetKey(VK_RIGHT))
	{
		offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		returnValue = i_currentValue < i_maxValue ? i_currentValue + offsetModifier : i_maxValue;
	}
	return returnValue;
}

float eae6320::Audio::ChangeSFXVolume(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue)
{
	float returnValue = i_currentValue;
	float offsetModifier;
	if (UserInput::GetKey(VK_LEFT))
	{
		offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		returnValue = i_currentValue > i_minValue ? i_currentValue - offsetModifier : i_minValue;
	}
	else if (UserInput::GetKey(VK_RIGHT))
	{
		offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		returnValue = i_currentValue < i_maxValue ? i_currentValue + offsetModifier : i_maxValue;
	}
	return returnValue;
}