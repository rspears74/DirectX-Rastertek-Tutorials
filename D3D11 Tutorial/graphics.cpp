#include "graphics.h"

Graphics::Graphics()
{
	_d3DGraphics = 0;
}

Graphics::Graphics(const Graphics&)
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;

	//create the D3DGraphics object
	_d3DGraphics = new D3DGraphics();
	if (!_d3DGraphics) { return false; }

	//init the D3DGraphics object
	result = _d3DGraphics->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//release the Direct3D object
	if (_d3DGraphics)
	{
		_d3DGraphics->Shutdown();
		delete _d3DGraphics;
		_d3DGraphics = 0;
	}

	return;
}

bool Graphics::Frame()
{
	bool result;

	//render the graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
{
	//clear the buffers to begin the scene
	_d3DGraphics->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//present the rendered scene to the screen
	_d3DGraphics->EndScene();

	return true;
}