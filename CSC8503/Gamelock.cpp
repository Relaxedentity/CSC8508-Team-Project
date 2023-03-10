#include "Gamelock.h"
using namespace NCL;
using namespace CSC8503;

bool GameLock::Player1lock;
bool GameLock::Player2lock;
bool GameLock::gamestart;
bool GameLock::gamePause;
int  GameLock::gamemod;
float GameLock::gametime;
float GameLock::redScore;
float GameLock::blueScore;
ISoundEngine* GameLock::hitsound;