#ifndef EAE6320_NETWORKMANAGER_H
#define EAE6320_NETWORKMANAGER_H
#include <cstdint>
#include <map>
#include "../../External/RakNet/4.081/RakNetTypes.h"

namespace eae6320
{
	namespace Math
	{
		class cVector;
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class Text;
		}
	}
}

namespace RakNet
{
	class RakPeerInterface;
	struct SocketDescriptor;
}
namespace eae6320
{
	namespace Gameplay
	{
		class GameObject;
	}
}

namespace eae6320
{
	namespace Network
	{
		enum struct SoundID2D;
		enum struct SoundID3D;
	}
}

namespace eae6320
{
	namespace Network
	{
		class NetworkManager
		{
		public:
			static bool Initialize(bool i_isServer, uint16_t i_serverPort = 60000, uint32_t i_maxClients = 10);
			void ProcessIncomingPackets();
			void Draw()const;
			static bool CleanUp();
			static NetworkManager* GetSingleton();
			static void AddToMap(const std::string i_key, Gameplay::GameObject* i_value);
			void TriggerMySoundsOnNetwork2D(const SoundID2D i_soundID2D) const;
			void TriggerMySoundsOnNetwork3D(const SoundID3D i_soundID3D, const Math::cVector i_position) const;
		private:
			static std::map<const std::string, Gameplay::GameObject*> networkGameObjects;
			static NetworkManager* singleton;
			Debug::UI::Text* m_myscore;
			Debug::UI::Text* m_otherscore;
			NetworkManager(bool i_isServer, uint16_t i_serverPort, uint32_t i_maxClients);
			RakNet::RakPeerInterface* m_rakPeerInterface;
			RakNet::RakNetGUID m_otherPlayersGUID;
			uint32_t m_maxClients;
			uint16_t m_serverPort;
			bool m_isServer;
			bool m_showRemotePlayer;
		};
	}
}

#endif // !EAE6320_NETWORKMANAGER_H