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
	namespace Audio
	{
		class AudioManager
		{
		public:		
			static bool Initialize(const int i_maxchannels = 32);
			static AudioManager* GetSingleton();
			static bool CleanUp();
			FMOD::System* GetFMODSystem()const;
		private:
			~AudioManager(){}
			static AudioManager* singleton;
			explicit AudioManager(const int i_maxchannels);
			const int m_maxchannels;
			FMOD::System *m_fmodSystem;
			bool m_isAudioEnabled;
		};
		extern std::map<const std::string, AudioClip*> audioClips;
	}
}

#endif // !EAE6320_AUDIOMANAGER_H
