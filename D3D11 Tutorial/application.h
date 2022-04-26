#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "input.h"
#include "graphics.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR _applicationName;
	HINSTANCE _hInstance;
	HWND _hWnd;

	Input* _input;
	Graphics* _graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Application* AppHandle = 0;

#endif