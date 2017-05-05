#ifndef EAE6320_AUDIOMANAGER_H
#define EAE6320_AUDIOMANAGER_H

#include <map>

namespace FMOD
{
	class System;
}

namespace eae6320
{
	namespace Audio
	{
		class AudioClip;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		struct Transform;
	}
}

namespace eae6320
{
	namespace Audio
	{
		class AudioManager
		{
		public:		
			static bool Initialize(const int i_maxchannels = 32);
			static AudioManager* GetSingleton();
			static bool CleanUp();
			static void Update();
			FMOD::System* GetFMODSystem()const;
			void SetCameraTransform(Gameplay::Transform const * const i_cameraTransform);
		private:
			~AudioManager(){}
			static AudioManager* singleton;
			Gameplay::Transform const * m_cameraTransform;
			explicit AudioManager(const int i_maxchannels);
			const int m_maxchannels;
			FMOD::System *m_fmodSystem;
			bool m_isAudioEnabled;
		};
		extern std::map<const std::string, AudioClip*> audioClips;		
		float ChangeMusicVolume(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue);
		float ChangeSFXVolume(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue);
		extern bool isWindowInFocus;
	}
}

#endif // !EAE6320_AUDIOMANAGER_H
