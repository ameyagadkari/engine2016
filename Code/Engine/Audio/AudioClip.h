#ifndef EAE6320_AUDIOCLIP_H
#define EAE6320_AUDIOCLIP_H

typedef unsigned int FMOD_MODE;
namespace FMOD
{
	class Channel;
	class Sound;
}

namespace eae6320
{
	namespace Audio
	{
		class AudioClip
		{
		public:
			AudioClip(char const * const i_path, const FMOD_MODE i_mode, const int i_channelID);
			~AudioClip();
			bool Play(const bool i_isLooped = false, const bool i_isPaused = false);
		private:		
			bool Initialize(char const * const i_path, const FMOD_MODE i_mode, const int i_channelID);
			bool CleanUp() const;
			FMOD::Sound* m_clip;
			FMOD::Channel* m_channel;
		};
	}
}

#endif // !EAE6320_AUDIOCLIP_H
