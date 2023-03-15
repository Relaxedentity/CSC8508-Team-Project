#include "Gamelock.h"
using namespace NCL;
using namespace CSC8503;

bool GameLock::Player1lock;
bool GameLock::Player2lock;
bool GameLock::gamestart;
int  GameLock::gamemod;
float GameLock::gametime;
ISoundEngine* GameLock::hitsound;
vector<vector<Vector2>> GameLock::IntroButtonPos;
vector<vector<Vector2>> GameLock::EndButtonPos;
vector<vector<Vector2>> GameLock::CoopButtonPos;
vector<vector<Vector2>> GameLock::BackButtonPos;

bool GameLock::normalBtnChange;
bool GameLock::coopBtnChange;
bool GameLock::introBtnChange;
bool GameLock::exitBtnChange;
bool GameLock::BackBtnChange;
bool GameLock::SingleExitBtnChange;
bool GameLock::CoopExitBtnChange;

bool GameLock::Mainmenuawake;
bool GameLock::SingleEndMenuawake;
bool GameLock::CoopEndMenuawake;
bool GameLock::IntroMenuawake;

bool GameLock::isloading1;
bool GameLock::isloading2;

int GameLock::loadingdone;