#ifndef EAE6320_AUDIOCLIP_H
#define EAE6320_AUDIOCLIP_H

namespace FMOD
{
	class Sound;
}

namespace eae6320
{
	namespace Audio
	{
		class AudioClip
		{
		public:
			bool Initialize(char const * const i_path);
			bool CleanUp();
			bool PlayOnce();
			bool PlayLooped();
		private:
			FMOD::Sound* m_clip;
		};
	}
}

#endif // !EAE6320_AUDIOCLIP_H
