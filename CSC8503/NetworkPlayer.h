#pragma once
#include "GameObject.h"
#include "GameClient.h"

namespace NCL {
	namespace CSC8503 {
		class NetworkedGame;

		class NetworkPlayer : public GameObject {
		public:
			NetworkPlayer(NetworkedGame* game, int num, GameWorld* world);
			~NetworkPlayer();
			void Update(float dt) override {};

			void OnCollisionBegin(GameObject* otherObject) override;

			int GetPlayerNum() const {
				return playerNum;
			}

		protected:
			NetworkedGame* game;
			int playerNum;
		};
	}
}

