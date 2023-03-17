#include "Gamepad.h"

GamePad::~GamePad()
{
}

int GamePad::GetPort()
{
	return controllerID + 1;
}

XINPUT_GAMEPAD* GamePad::GetState()
{
	return &state.Gamepad;
}

bool GamePad::CheckConnection()
{
	int controllerId = -1;
	DWORD dwResult;

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS) {
			controllerId = i;

			std::cout << "Controller is Connected" << std::endl;
		}
		else
			std::cout << "Controller is not connected" << std::endl;
	}

	controllerID = controllerId;

	return controllerId != -1;
}

// true if controller is connected
bool GamePad::UpdateController()
{
	if (controllerID == -1)
		CheckConnection();

	if (controllerID != -1)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(controllerID, &state) != ERROR_SUCCESS)
		{
			controllerID = -1;
			return false;
		}

		// get percentage of x and y's movement 
		float normLStickX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
		float normLStickY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);

		// normalising t
		leftStickX = (abs(normLStickX) < deadzoneX ? 0 : (abs(normLStickX) - deadzoneX) * (normLStickX / abs(normLStickX)));
		leftStickY = (abs(normLStickY) < deadzoneY ? 0 : (abs(normLStickY) - deadzoneY) * (normLStickY / abs(normLStickY)));

		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);

		float normRStickX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
		float normRStickY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);

		rightStickX = (abs(normRStickX) < deadzoneX ? 0 : (abs(normRStickX) - deadzoneX) * (normRStickX / abs(normRStickX)));
		rightStickY = (abs(normRStickY) < deadzoneY ? 0 : (abs(normRStickY) - deadzoneY) * (normRStickY / abs(normRStickY)));

		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);

		leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)state.Gamepad.bRightTrigger / 255;

		return true;
	}
	return false;
}

bool GamePad::IsPressed(WORD button)
{
	return (state.Gamepad.wButtons & button) != 0;
}
