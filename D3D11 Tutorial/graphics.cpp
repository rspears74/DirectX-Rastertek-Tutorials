#include "graphics.h"

Graphics::Graphics()
{
	_d3DGraphics = 0;
	_camera = 0;
	_model = 0;
	_colorShaders = 0;
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

	//create the camera
	_camera = new Camera();
	if (!_camera) { return false; }

	//set the initial position of the camera
	_camera->SetPosition(0.0f, 0.0f, -5.0f);

	//create the model
	_model = new Model();
	if (!_model) { return false; }

	//initialize the model
	result = _model->Initialize(_d3DGraphics->GetDevice());
	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	//create the color shaders
	_colorShaders = new ColorShaders();
	if (!_colorShaders) { return false; }

	//init the color shaders
	result = _colorShaders->Initialize(_d3DGraphics->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize the color shaders", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//release the color shaders
	if (_colorShaders)
	{
		_colorShaders->Shutdown();
		delete _colorShaders;
		_colorShaders = 0;
	}

	//release the model
	if (_model)
	{
		_model->Shutdown();
		delete _model;
		_model = 0;
	}

	//release the camera
	if (_camera)
	{
		delete _camera;
		_camera = 0;
	}

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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//clear the buffers to begin the scene
	_d3DGraphics->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//generate the view matrix based on the camera's position
	_camera->Render();

	//get the world, view, and projection matrices from the camera and d3d objects
	_d3DGraphics->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	_d3DGraphics->GetProjectionMatrix(projectionMatrix);

	//put the model verted and index buffers on the graphics pipeline to prepare them for drawing
	_model->Render(_d3DGraphics->GetDeviceContext());

	//render the model using the color shader
	result = _colorShaders->Render(_d3DGraphics->GetDeviceContext(), _model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) { return false; }

	//present the rendered scene to the screen
	_d3DGraphics->EndScene();

	return true;
}