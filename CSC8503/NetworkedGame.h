#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);
			

			void UpdateGame(float dt) override;

			//void SpawnPlayer();

			//void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);
			int worldCount;
			int netID;
			int playerDistance;
			
			/*int worldCount;
			int netID;
			int playerDistance;*/

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);
			void UpdateNetworkAnimations(float dt);
			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();
			void ResetMovementFrame(GameObject* p);
			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;
			int lastID;

			std::vector<NetworkObject*> networkObjects;

			std::map<int, GameObject*> serverPlayers;
			GameObject* localPlayer;
			
			OGLShader* animatedShaderB = nullptr;
			OGLShader* animatedShaderC = nullptr;
			OGLShader* animatedShaderD = nullptr;
		};
	}
}

