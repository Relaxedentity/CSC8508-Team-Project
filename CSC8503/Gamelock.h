#pragma once
#include "Sound.h"
namespace NCL {
	namespace CSC8503 {
		class GameLock {
		public:
			static bool Player1lock;
			static bool Player2lock;
			static float gametime;
			static float redScore;
			static float blueScore;
			static bool gamestart;
			static bool gamePause;
			static int gamemod;
			static const int gamelength = 15;
			static ISoundEngine* hitsound;
		};
	}
}