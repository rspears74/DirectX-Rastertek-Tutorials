#include "d3dgraphics.h"

D3DGraphics::D3DGraphics()
{
	_swapChain = 0;
	_device = 0;
	_deviceContext = 0;
	_renderTargetView = 0;
	_depthStencilBuffer = 0;
	_depthStencilState = 0;
	_depthStencilView = 0;
	_rasterState = 0;
}

D3DGraphics::D3DGraphics(const D3DGraphics& other)
{

}

D3DGraphics::~D3DGraphics()
{

}

bool D3DGraphics::Initialize(int screenWidth, int screenHeight, bool vSync, HWND hWnd, bool fullScreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory1* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	
	//store the vsync setting
	_vSyncEnabled = vSync;

	//create a DirectX graphics interface factory
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
	if (FAILED(result)) { return false; }

	//use the factory to create and adapter for the primary graphics interface (video card)
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) { return false; }

	//enumerate the primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) { return false; }

	//get the number of modes that fir the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) { return false; }

	//create a list to hold all the possible display modes for this monitor/video card combo
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) { return false; }

	//now fill the display mode list structures
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) { return false; }

	//now go through all the display modes and find the one that matches the screen width and height
	//when a match is found, store the numerator and denominator of the refresh rate for that monitor
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//get the adapter (video card) description
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) { return false; }

	//store the dedicated video card memory in megabytes
	_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//convert the name of the video card into a char array and store it
	error = wcstombs_s(&stringLength, _videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) { return false; }

	//release the display mode list
	delete[] displayModeList;
	displayModeList = 0;
	
	//release the adapter output
	adapterOutput->Release();
	adapterOutput = 0;

	//release the adapter
	adapter->Release();
	adapter = 0;

	//release the factory
	factory->Release();
	factory = 0;

	//initialize the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set to a single back buffer
	swapChainDesc.BufferCount = 1;

	//set the width and height of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//set the regular 32-bit surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//set the refresh rate of the back buffer
	if (_vSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//set the usage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//set the handle for the window to render to
	swapChainDesc.OutputWindow = hWnd;

	//turn multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//set to fullscreen or windowed
	if (fullScreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//set the scan line ordering and scaling to unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//no advanced flags
	swapChainDesc.Flags = 0;

	//set the feature level to DirectX 11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//create the swap chain, D3D device and D3D device context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
	if (FAILED(result)) { return false; }

	//get the pointer to the back buffer
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) { return false; }

	//create the render target view with the back buffer pointer
	result = _device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView);
	if (FAILED(result)) { return false; }

	//release the pointer to the back buffer as we no longer need it
	backBufferPtr->Release();
	backBufferPtr = 0;

	//init the desciption of the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//set up the description of the depth buffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//create the texture for the depth buffer using the filled out description
	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
	if (FAILED(result)) { return false; }

	//init the description of stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//setup the desc of the stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//stencil operations if pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//stencil operations if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create the dpeth stencil state
	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	if (FAILED(result)) { return 0; }

	//set the depth stencil state
	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

	//init the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//setup the depth stencil view desc
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//create the depth stencil view
	result = _device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	if (FAILED(result)) { return false; }

	//bind the render target view and depth stencil buffer to the output render pipeline
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	//setup the raster description, which will determine howo and what polygons will be drawn
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//create the rasterizer state from the desc
	result = _device->CreateRasterizerState(&rasterDesc, &_rasterState);
	if (FAILED(result)) { return false; }

	_deviceContext->RSSetState(_rasterState);

	//setup the viewport for rendering
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//create the viewport
	_deviceContext->RSSetViewports(1, &viewport);

	//setup the projectionMatrix
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//create the projection matrix for 3D rendering
	_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//init the world matrix to the identity matrix
	_worldMatrix = XMMatrixIdentity();

	//create an oethographic projection matrix for 2D rendering
	_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DGraphics::Shutdown()
{
	//before shutting down set to windowed mode or when you releast the swap chain it will throw an exception
	if (_swapChain)
	{
		_swapChain->SetFullscreenState(false, NULL);
	}

	if (_rasterState)
	{
		_rasterState->Release();
		_rasterState = 0;
	}

	if (_depthStencilView)
	{
		_depthStencilView->Release();
		_depthStencilView = 0;
	}

	if (_depthStencilState)
	{
		_depthStencilState->Release();
		_depthStencilState = 0;
	}

	if (_depthStencilBuffer)
	{
		_depthStencilBuffer->Release();
		_depthStencilBuffer = 0;
	}

	if (_renderTargetView)
	{
		_renderTargetView->Release();
		_renderTargetView = 0;
	}

	if (_deviceContext)
	{
		_deviceContext->Release();
		_deviceContext = 0;
	}

	if (_device)
	{
		_device->Release();
		_device = 0;
	}

	if (_swapChain)
	{
		_swapChain->Release();
		_swapChain = 0;
	}

	return;
}

void D3DGraphics::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//setup the color to clear the buffer to
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//clear the back buffer
	_deviceContext->ClearRenderTargetView(_renderTargetView, color);

	//clear the depth buffer
	_deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DGraphics::EndScene()
{
	//present the back buffer to the screen since rendering is complete
	if (_vSyncEnabled)
	{
		//lock to screen refresh rate
		_swapChain->Present(1, 0);
	}
	else
	{
		//present as fast as possible
		_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DGraphics::GetDevice() { return _device; }

ID3D11DeviceContext* D3DGraphics::GetDeviceContext() { return _deviceContext; }

void D3DGraphics::GetProjectionMatrix(XMMATRIX& projMatrix)
{
	projMatrix = _projectionMatrix;
	return;
}

void D3DGraphics::GetWorldMatrix(XMMATRIX& wMatrix)
{
	wMatrix = _worldMatrix;
	return;
}

void D3DGraphics::GetOrthoMatrix(XMMATRIX& oMatrix)
{
	oMatrix = _orthoMatrix;
	return;
}

void D3DGraphics::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, _videoCardDescription);
	memory = _videoCardMemory;
	return;
}