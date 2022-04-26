#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <Windows.h>
#include "d3dgraphics.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
	D3DGraphics* _d3DGraphics;

};

#endif
