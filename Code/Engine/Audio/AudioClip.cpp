#include "AudioClip.h"
#include "AudioManager.h"
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"

namespace
{
	FMOD::System* fmodSystem = eae6320::Audio::AudioManager::GetSingleton()->GetFMODSystem();
}

bool eae6320::Audio::AudioClip::Initialize(char const * const i_path)
{
	fmodSystem->createSound(i_path, , nullptr, &m_clip);
}