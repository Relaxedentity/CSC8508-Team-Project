#include "Gamelock.h"
using namespace NCL;
using namespace CSC8503;

bool GameLock::Player1lock;
bool GameLock::Player2lock;
bool GameLock::gamestart;
int  GameLock::gamemod;
float GameLock::gametime;
ISoundEngine* GameLock::hitsound;
vector<vector<Vector2>> GameLock::buttonPos;
bool GameLock::normalBtnChange = false;
bool GameLock::coopBtnChange = false;
bool GameLock::introBtnChange = false;
bool GameLock::exitBtnChange = false;