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
		class AudioManager
		{
		public:
			static bool Initialize();
			static AudioManager* GetSingleton();
			static bool CleanUp();
		private:
			static AudioManager* singleton;
			AudioManager();
			FMOD::System *fmodSystem;
			bool isAudioEnabled;
		};
	}
}

#endif // !EAE6320_AUDIOMANAGER_H
