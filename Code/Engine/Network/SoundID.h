#ifndef EAE6320_SOUNDID_H
#define EAE6320_SOUNDID_H

namespace eae6320
{
	namespace Network
	{
		enum struct SoundID2D
		{
			PLAY_MY_TEAM_FLAG_RESET,
			PLAY_MY_TEAM_FLAG_PICKED,
			PLAY_OTHER_TEAM_SCORED,
		};
		enum struct SoundID3D
		{
			PLAY_OTHER_HEAVY_BREATHING,
			PLAY_OTHER_RUNNING,
			PLAY_OTHER_WALKING,
		};
	}
}

#endif // !EAE6320_SOUNDID_H