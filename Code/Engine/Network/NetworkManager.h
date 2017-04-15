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
	namespace Network
	{
		class NetworkManager
		{
		public:
			static bool Initialize(bool i_isServer, uint16_t i_serverPort = 60000, uint32_t i_maxClients = 10);
			static bool CleanUp();
			static NetworkManager* GetSingleton();
		private:
			static NetworkManager* singleton;
			NetworkManager(bool i_isServer, uint16_t i_serverPort, uint32_t i_maxClients);
			RakNet::RakPeerInterface* m_rakPeerInterface;
			uint32_t m_maxClients;
			uint16_t m_serverPort;
			bool m_isServer;
		};
	}
}

#endif // !EAE6320_NETWORKMANAGER_H