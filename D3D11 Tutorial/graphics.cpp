#include "graphics.h"

Graphics::Graphics()
{

}

Graphics::Graphics(const Graphics&)
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hWnd)
{
	return true;
}

void Graphics::Shutdown()
{
	return;
}

bool Graphics::Frame()
{
	return true;
}

bool Graphics::Render()
{
	return true;
}