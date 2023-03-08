#pragma once
#include "Sound.h"
namespace NCL {
	namespace CSC8503 {
		class GameLock {
		public:
			static bool Player1lock;
			static bool Player2lock;
			static float gametime;
			static bool gamestart;
			static int gamemod;
			static ISoundEngine* hitsound;
		};
	}
}