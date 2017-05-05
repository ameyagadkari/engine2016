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
#include "../../Game/Gameplay/Transform.h"
#include <regex>


namespace
{
	void GenerateRelativePaths(std::string prefix);
	std::vector<std::string> relativePaths;
	std::vector<std::string> fileNames;
	const std::regex pattern_match("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");
	const float speed_unitsPerSecond = 0.75f;
	eae6320::Audio::AudioClip* backgroudMusic = nullptr;
	eae6320::Math::cVector previousPosition;
	const float distanceFactor = 3779.527f;
	void ConvertFromcVectorToFMOD_Vector(const eae6320::Math::cVector i_input, FMOD_VECTOR& o_output);
}

namespace eae6320
{
	namespace Audio
	{
		std::map<const std::string, AudioClip*> audioClips;
		bool isWindowInFocus = false;
	}
}

eae6320::Audio::AudioManager* eae6320::Audio::AudioManager::singleton = nullptr;

eae6320::Audio::AudioManager::AudioManager(const int i_maxchannels) :
	m_cameraTransform(nullptr),                                                                     
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

		// Set 3D Settings
		{
			const FMOD_RESULT result = singleton->m_fmodSystem->set3DSettings(1.0, distanceFactor, 1.0f);
			if (result != FMOD_OK)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, FMOD_ErrorString(result));
				Logging::OutputError("Failed to set 3D settings of FMOD system %s", FMOD_ErrorString(result));
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
		backgroudMusic = audioClips.at("backgroundmusic");
		const bool isLooped = true;
		backgroudMusic->Play(isLooped);
		backgroudMusic->SetVolume(0.25f);
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
	backgroudMusic->SetVolume(returnValue);
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
	const float musicVolume = backgroudMusic->GetVolume();
	for (auto& audioClip : audioClips)
	{
		audioClip.second->SetVolume(returnValue);
	}
	backgroudMusic->SetVolume(musicVolume);
	return returnValue;
}

void eae6320::Audio::AudioManager::SetCameraTransform(Gameplay::Transform const * const i_cameraTransform)
{
	m_cameraTransform = i_cameraTransform;
	previousPosition = m_cameraTransform->m_position;
}

void eae6320::Audio::AudioManager::Update()
{
	if (backgroudMusic)
	{
		if (backgroudMusic->GetPaused() && isWindowInFocus)
		{
			backgroudMusic->SetPaused(false);
		}
		else if (!backgroudMusic->GetPaused() && !isWindowInFocus)
		{
			backgroudMusic->SetPaused(true);
		}
	}

	if(singleton->m_cameraTransform)
	{
		FMOD_VECTOR currentPosition;
		ConvertFromcVectorToFMOD_Vector(singleton->m_cameraTransform->m_position, currentPosition);

		FMOD_VECTOR currentVelocity;
		ConvertFromcVectorToFMOD_Vector(singleton->m_cameraTransform->m_position - previousPosition, currentVelocity);

		FMOD_VECTOR currentForward;
		ConvertFromcVectorToFMOD_Vector(singleton->m_cameraTransform->m_localAxes.m_forward, currentForward);

		FMOD_VECTOR currentUp;
		ConvertFromcVectorToFMOD_Vector(singleton->m_cameraTransform->m_localAxes.m_up, currentUp);

		const FMOD_RESULT result = singleton->m_fmodSystem->set3DListenerAttributes(
			0, 
			&currentPosition, 
			&currentVelocity, 
			&currentForward,
			&currentUp);
		if (result != FMOD_OK)
		{
			EAE6320_ASSERTF(false, FMOD_ErrorString(result));
			Logging::OutputError("Failed to set 3D Listener Attributes %s", FMOD_ErrorString(result));
		}
	}

	const FMOD_RESULT result = singleton->m_fmodSystem->update();
	if (result != FMOD_OK)
	{
		EAE6320_ASSERTF(false, FMOD_ErrorString(result));
		Logging::OutputError("Failed to update FMOD system %s", FMOD_ErrorString(result));
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