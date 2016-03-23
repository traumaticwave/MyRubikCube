#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <windows.h>
#include <wchar.h>

#include "resource.h"

#include "d3dClass.h"
#include "Myobject.h"
#include "shaderClass.h"
#include "textureClass.h"
#include "processClass.h"
#include "fontClass.h"
#include "timer.h"
#include "inputClass.h"

LPCTSTR WndClassName = L"Trauma";
HWND hwnd = NULL;
HINSTANCE hInst = NULL;

bool INPUT_BLOCKED = false;
static char szSentence[50];

// Functions
bool UpdateScene(float);
void DrawScene();
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int messageloop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
using namespace DirectX;

// Classes
d3dClass *d3d_h = new d3dClass;
MyObject *object_h = new MyObject;
shaderClass *shader_h = new shaderClass;
textureClass *texture_h = new textureClass;
processClass *process_h = new processClass;
fontClass *font_h = new fontClass;
inputClass *input_h = new inputClass;

int WINAPI WinMain(HINSTANCE hInstance,	//Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	if (!InitializeWindow(hInstance, nShowCmd, 1024, 768, true))
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!d3d_h->initialize(hwnd, hInstance))	//Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!object_h->initialize(d3d_h->getDevice(), d3d_h->getContext())) {
		MessageBox(0, L"Object Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!shader_h->initialize(d3d_h->getDevice(), d3d_h->getContext()))	//Initialize our scene
	{
		MessageBox(0, L"Shader Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!texture_h->initialize(d3d_h->getDevice(), d3d_h->getContext()))	//Initialize our scene
	{
		MessageBox(0, L"Texture Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!process_h->initialize(1024, 768, d3d_h->getDevice()))	//Initialize our scene
	{
		MessageBox(0, L"Logic Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!font_h->initialize(d3d_h->getDevice(), d3d_h->getContext())) {
		MessageBox(0, L"Font Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!input_h->initialize(hInst, hwnd)) {
		MessageBox(0, L"Input Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	d3d_h->zbuffer_on();
	messageloop();

	input_h->clean();
	font_h->clean();
	process_h->clean();
	texture_h->clean();
	shader_h->clean();
	object_h->clean();
	d3d_h->clean();
	return 0;
}

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	hInst = hInstance;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WndClassName;

	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}


	RECT rt = { 0, 0, width, height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int startX = (screenWidth - (rt.right - rt.left))/2;
	int startY = (screenHeight - (rt.bottom - rt.top)) / 2;
	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"돌기둥의 Rubik's Cube Puzzle-세월호 희생자를 추모하며",
		WS_OVERLAPPEDWINDOW,
		startX, startY,
		(rt.right - rt.left), (rt.bottom - rt.top),
		NULL, NULL, hInstance, NULL
		);

	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

bool UpdateScene(float time)
{
	static int c;
	process_h->update(time, input_h->detect_mouse_input());
	if (INPUT_BLOCKED == false) {
		c = input_h->detect_keyboard_input(hwnd, time);
		if( 0 < c )
			INPUT_BLOCKED = true;
	}

	if (INPUT_BLOCKED == true) {
		switch (c) {
		case 1:
			object_h->rotation_right(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 2:
			object_h->rotation_left(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 3:
			object_h->rotation_up(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 4:
			object_h->rotation_front(d3d_h->getContext(), &INPUT_BLOCKED);
			break;
////////

		case 5:
			object_h->reverse_rotation_right(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 6:
			object_h->reverse_rotation_left(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 7:
			object_h->reverse_rotation_up(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 8:
			object_h->reverse_rotation_front(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

///////

		case 9:
			object_h->total_rotation_X(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 11:
			object_h->total_rotation_Y(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 12:
			object_h->total_rotation_Z(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 13:
			object_h->total_reverse_rotation_X(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 15:
			object_h->total_reverse_rotation_Y(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		case 16:
			object_h->total_reverse_rotation_Z(d3d_h->getContext(), &INPUT_BLOCKED);
			break;

		default:
			break;
		}
	}

	//font_h->UpdateSentence(m_sentence, "Hello world!", 100, 100, 1.0f, 1.0f, 1.0f, context); // 차후에 수정
	return true;
}

void DrawScene()
{
	d3d_h->zbuffer_on();
	d3d_h->begin();   //OK!

	object_h->render(d3d_h->getDevice(), d3d_h->getContext());
	shader_h->render(d3d_h->getDevice(), d3d_h->getContext());

	///////////////**************new**************////////////////////

	process_h->render(2, d3d_h->getDevice(), d3d_h->getContext());
	texture_h->setBlend(d3d_h->getDevice(), d3d_h->getContext());
	for (int count = 0; count < 27; count++) {
		for (int index = 0; index < 6; index++) {
			texture_h->counterclock_render(d3d_h->getDevice(), d3d_h->getContext(), index);
			d3d_h->getContext()->DrawIndexed(6, 36 * count + 6 * index, 0);
		}
	}

	for (int count = 0; count < 27; count++) {
		for (int index = 0; index < 6; index++) {
			texture_h->clock_render(d3d_h->getDevice(), d3d_h->getContext(), index);
			d3d_h->getContext()->DrawIndexed(6, 36 * count + 6 * index, 0);
		}
	}

	font_h->SetShaderParameters(d3d_h->getContext(),
		XMMatrixIdentity(),
		XMMatrixIdentity(),     //process_h->camView, 
		process_h->ortho, 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	d3d_h->zbuffer_off();
	font_h->RenderShader(d3d_h->getContext(), font_h->getIndexCount());

	d3d_h->end(); //OK!
}

int messageloop() 
{
	static MSG msg;
	static bool set = false;
	char temp[20];

	ZeroMemory(&msg, sizeof(MSG));
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			frameCount++;
			if (GetTime() > 1.0f) {
				StartTimer();
				frameTime = GetFrameTime();
				//_fcvt_s(szSentence, frameTime, 10, &dec, &sign);
				//mbstowcs(szSentence2, szSentence, 30);
				strcpy(szSentence, " fps : ");
				_itoa_s(frameCount, temp, 20, 10);
				strcat(szSentence, temp);
				font_h->UpdateSentence(d3d_h->getContext(), szSentence, 20, 20, 1.0f, 1.0f, 1.0f );
				frameCount = 0;
			}
			UpdateScene(frameTime);
			DrawScene();
		}
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	WCHAR fullpath[1024] = L"";
	WCHAR filename[1024] = L"";

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_COMMAND:
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = fullpath;
		ofn.nMaxFile = 1024;
		ofn.lpstrFileTitle = filename;
		ofn.nMaxFileTitle = 1024;

		switch (LOWORD(wParam)) {
		case ID_UPPER_FACE:
			if (GetOpenFileName(&ofn)) {
				MessageBox(NULL, filename, L"sssss", MB_OK);
				texture_h->change_image(d3d_h->getDevice(), filename, 2);
			}
			return 0;

		case ID_LOWER_FACE:
			if (GetOpenFileName(&ofn)) {
				MessageBox(NULL, filename, L"sssss", MB_OK);
				texture_h->change_image(d3d_h->getDevice(), filename, 3);
			}
			return 0;

		case ID_LEFT_FACE:
			if (GetOpenFileName(&ofn)) {
				MessageBox(NULL, filename, L"sssss", MB_OK);
				texture_h->change_image(d3d_h->getDevice(), filename, 4);
			}
			return 0;

		case ID_RIGHT_FACE:
			if (GetOpenFileName(&ofn)) {
				MessageBox(NULL, filename, L"sssss", MB_OK);
				texture_h->change_image(d3d_h->getDevice(), filename, 5);
			}
			return 0;

		default:
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

