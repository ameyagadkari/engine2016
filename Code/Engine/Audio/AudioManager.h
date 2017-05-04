#ifndef EAE6320_AUDIOMANAGER_H
#define EAE6320_AUDIOMANAGER_H

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
			static bool Initialize();
			static AudioManager* GetSingleton();
			static bool CleanUp();
			FMOD::System* GetFMODSystem()const;
		private:
			~AudioManager(){}
			static AudioManager* singleton;
			AudioManager();
			AudioClip * m_backgroundMusic;
			FMOD::System *m_fmodSystem;
			bool m_isAudioEnabled;
		};
	}
}

#endif // !EAE6320_AUDIOMANAGER_H
