#include "inputClass.h"

inputClass::inputClass()
{
}

inputClass::~inputClass()
{
}

bool inputClass::initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard,NULL);
	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

int inputClass::detect_keyboard_input(HWND hwnd, double time)
{
	DIKeyboard->Acquire();
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80){
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	//우측면 동작들
	else if ( keyboardState[DIK_RIGHT] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80) ) {
		return 1;
	}
	else if (keyboardState[DIK_RIGHT] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 5;
	}
	// 좌측면 동작들
	else if (keyboardState[DIK_LEFT] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 2;
	}
	else if (keyboardState[DIK_LEFT] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 6;
	}
	// 윗면 동작
	else if (keyboardState[DIK_UP] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 3;
	}
	else if (keyboardState[DIK_UP] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 7;
	}
	// 정면 동작들
	else if (keyboardState[DIK_DOWN] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 4;
	}
	else if (keyboardState[DIK_DOWN] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 8;
	}

	// 전체 x축 회전
	else if (keyboardState[DIK_D] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 9;
	}
	// 전체 y축 회전
	else if (keyboardState[DIK_W] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 11;
	}
	// 전체 Z축 회전
	else if (keyboardState[DIK_S] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 12;
	}
	else if (keyboardState[DIK_A] & 0x80 && !(keyboardState[DIK_LSHIFT] & 0x80)) {
		return 13;
	}
	else if (keyboardState[DIK_W] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 15;
	}
	else if (keyboardState[DIK_S] & 0x80 && (keyboardState[DIK_LSHIFT] & 0x80)) {
		return 16;
	}
	return 0;
}

float inputClass::detect_mouse_input()
{
	DIMOUSESTATE mouseCurrState;
	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	if ((mouseLastState.rgbButtons[0] & 0x80) && (mouseCurrState.rgbButtons[0] & 0x80)) {
		rot_y += 0.01f * mouseCurrState.lX;
	}
	mouseLastState = mouseCurrState;

	if (rot_y >= XM_PIDIV4) {
		rot_y = XM_PIDIV4;
	}
	if (rot_y <= -XM_PIDIV4) {
		rot_y = -XM_PIDIV4;
	}
	return rot_y;
}


void inputClass::clean()
{
	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
}