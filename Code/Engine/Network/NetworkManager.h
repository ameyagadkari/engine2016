#ifndef EAE6320_NETWORKMANAGER_H
#define EAE6320_NETWORKMANAGER_H
#include <cstdint>
#include <map>

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
			static void AddToMap(const std::string i_key, Gameplay::GameObject* i_value);
			/*static Gameplay::GameObject* nativePlayer;
			static Gameplay::GameObject* remotePlayer;

			static Gameplay::GameObject* myteamflagserver;
			static Gameplay::GameObject* otherteamflagserver;

			static Gameplay::GameObject* myteamflagclient;
			static Gameplay::GameObject* otherteamflagclient;*/
		private:
			static std::map<const std::string, Gameplay::GameObject*> networkGameObjects;
			static NetworkManager* singleton;			
			NetworkManager(bool i_isServer, uint16_t i_serverPort, uint32_t i_maxClients);
			RakNet::RakPeerInterface* m_rakPeerInterface;
			uint32_t m_maxClients;
			uint16_t m_serverPort;
			bool m_isServer;
			bool m_showRemotePlayer;
		};
	}
}

#endif // !EAE6320_NETWORKMANAGER_H