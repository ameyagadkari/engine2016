#ifndef EAE6320_NETWORKMANAGER_H
#define EAE6320_NETWORKMANAGER_H
#include <cstdint>

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
		class NetworkManager
		{
		public:
			static bool Initialize(bool i_isServer, uint16_t i_serverPort = 60000, uint32_t i_maxClients = 10);
			void ProcessIncomingPackets();
			void Draw()const;
			static bool CleanUp();
			static NetworkManager* GetSingleton();
			static Gameplay::GameObject* nativePlayer;
		private:
			static NetworkManager* singleton;
			Gameplay::GameObject* m_remotePlayer;		
			NetworkManager(bool i_isServer, uint16_t i_serverPort, uint32_t i_maxClients);
			RakNet::RakPeerInterface* m_rakPeerInterface;
			uint32_t m_maxClients;
			uint16_t m_serverPort;
			bool m_isServer;
		};
	}
}

#endif // !EAE6320_NETWORKMANAGER_H