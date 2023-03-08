#pragma once
#include <Xinput.h>
#include <minwinbase.h>
#include <winerror.h>
#include <iostream>

class GamePad
{
public:
	GamePad() : deadzoneX(0.05f), deadzoneY(0.05f) {}
	~GamePad();
	GamePad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY) {}

	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;

	int  GetPort();
	XINPUT_GAMEPAD* GetState();
	bool CheckConnection();
	bool UpdateController();
	bool IsPressed(WORD word);

private:
	int controllerID;
	XINPUT_STATE state;

	float deadzoneX;
	float deadzoneY;
};
