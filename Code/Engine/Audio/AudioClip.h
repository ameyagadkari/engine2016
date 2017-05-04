#ifndef EAE6320_AUDIOCLIP_H
#define EAE6320_AUDIOCLIP_H

typedef unsigned int FMOD_MODE;
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
			AudioClip(char const * const i_path, const FMOD_MODE i_mode);		
			~AudioClip();
			bool Play(const bool i_isLooped = false, const bool i_isPaused = false) const;
		private:		
			bool Initialize(char const * const i_path, const FMOD_MODE i_mode);
			bool CleanUp() const;
			FMOD::Sound* m_clip;
		};
	}
}

#endif // !EAE6320_AUDIOCLIP_H
