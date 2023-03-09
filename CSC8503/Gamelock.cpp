#include "Gamelock.h"
using namespace NCL;
using namespace CSC8503;

bool GameLock::Player1lock;
bool GameLock::Player2lock;
bool GameLock::gamestart;
int  GameLock::gamemod;
float GameLock::gametime;
ISoundEngine* GameLock::hitsound;