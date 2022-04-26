#include "application.h"

Application::Application()
{
	input = 0;
	graphics = 0;
}

Application::Application(const Application& other)
{

}

Application::~Application()
{

}

bool Application::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	input = new Input();
	if (!input) {
		return false;
	}

	input->Initialize();

	graphics = new Graphics();
	if (!graphics) {
		return false;
	}

	result = graphics->Initialize(screenWidth, screenHeight, hWnd);
	if (!result) {
		return false;
	}
}

void Application::Shutdown()
{
	if (graphics) {
		graphics->Shutdown();
		delete graphics;
		graphics = 0;
	}

	if (input) {
		delete input;
		input = 0;
	}

	ShutdownWindows();

	return;
}

void Application::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}

	return;
}

bool Application::Frame()
{
	bool result;

	if (input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
		{
			input->KeyDown((unsigned int)wParam);
			return 0;
		}

		case WM_KEYUP:
		{
			input->KeyUp((unsigned int)lParam);
			return 0;
		}

		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}

void Application::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//get external pointer to this object
	AppHandle = this;

	//get application instance
	hInstance = GetModuleHandle(NULL);

	//give the app a name
	applicationName = L"Engine";

	//setup the windows class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	//Determine the resolution of the desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//setup the screen settings depending on whether it is running in full screen or in windowed mode
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//set the position of the window to the top left corner
		posX = posY = 0;
	}
	else
	{
		//if windowed, set to 600x800 resolution
		screenWidth = 800;
		screenHeight = 600;

		//place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//create the window with the screen settings and get the handle to it
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	//bring the window up on the screen and set it as main focus
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	//hide the mouse cursor
	ShowCursor(false);

	return;
}

void Application::ShutdownWindows()
{
	//Show the mouse cursor
	ShowCursor(true);

	//fix the display settings if leaving full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//remove the window
	DestroyWindow(hWnd);
	hWnd = NULL;

	//remove the application instance
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	//release the pointer to the class
	AppHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		//all other messages pass to the message handler in the system class
		default:
		{
			return AppHandle->MessageHandler(hWnd, uMessage, wParam, lParam);
		}
	}
}
