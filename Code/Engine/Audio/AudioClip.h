#ifndef EAE6320_AUDIOCLIP_H
#define EAE6320_AUDIOCLIP_H

#include "../Math/cVector.h"

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
			void Play(const bool i_isLooped = false, const bool i_isPaused = false);
			void Play3D(const bool i_isLooped = false, const bool i_isPaused = false, const Math::cVector i_position = Math::cVector::zero, const Math::cVector i_velocity = Math::cVector::zero);
			void SetVolume(const float i_value) const;
			float GetVolume() const;
			void SetPaused(const bool i_value) const;
			bool GetIsPlaying() const;
			bool GetPaused() const;
			void Stop() const;
		private:
			bool Initialize(char const * const i_path, const FMOD_MODE i_mode, const int i_channelID);
			bool CleanUp() const;
			FMOD::Sound* m_clip;
			FMOD::Channel* m_channel;
		};
	}
}

#endif // !EAE6320_AUDIOCLIP_H
