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
			static int gamemod;
			static const int gamelength = 200;
			static ISoundEngine* hitsound;
			static vector<vector<Vector2>> IntroButtonPos;
			static vector<vector<Vector2>> EndButtonPos;
			static vector<vector<Vector2>> CoopButtonPos;
			static vector<vector<Vector2>> BackButtonPos;
			static bool normalBtnChange;
			static bool coopBtnChange;
			static bool introBtnChange;
			static bool exitBtnChange;
			static bool BackBtnChange;

			static bool SingleExitBtnChange;
			static bool CoopExitBtnChange;
			static bool BacktoMainMenu;

			static bool Mainmenuawake;
			static bool SingleEndMenuawake;
			static bool CoopEndMenuawake;
			static bool IntroMenuawake;

			static bool isloading1;
			static bool isloading2;
			static int loadingdone;

			static float p1ModeTime;
			static float p2ModeTime;

			static bool istoString;
			static bool isto2String;

			static bool isNetwork;

			static int RedS;
			static int BlueS;
		};
	}
}