#include "d3dClass.h"

d3dClass::d3dClass()
{
}

d3dClass::~d3dClass()
{
}

ID3D11Device *d3dClass::getDevice()
{
	return device;
}

ID3D11DeviceContext *d3dClass::getContext()
{
	return context;
}

bool d3dClass::begin()
{
	float bgColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, bgColor);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return true;
}

void d3dClass::end()
{
	//Present the backbuffer to the screen
	swap_chain->Present(1, 0);
}

bool d3dClass::initialize(HWND hwnd, HINSTANCE hInstance)
{
	HRESULT hr;

	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL,
		NULL, NULL,
		D3D11_SDK_VERSION,
		&swapChainDesc, &swap_chain,
		&device, NULL, &context);

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	hr = device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Set our Render Target
	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	context->RSSetViewports(1, &viewport);


	D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	depthStateDesc.DepthEnable = true;
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStateDesc.StencilEnable = true;
	depthStateDesc.StencilReadMask = 0xFF; 
	depthStateDesc.StencilWriteMask = 0xFF;

	depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState(&depthStateDesc, &depthState);


	D3D11_DEPTH_STENCIL_DESC depthDisableStateDesc;
	depthDisableStateDesc.DepthEnable = false;
	depthDisableStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisableStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisableStateDesc.StencilEnable = true;
	depthDisableStateDesc.StencilReadMask = 0xFF;
	depthDisableStateDesc.StencilWriteMask = 0xFF;

	depthDisableStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisableStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisableStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisableStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState(&depthDisableStateDesc, &depthDisableState);

	return true;
}

void d3dClass::clean()
{
	//Release the COM Objects we created
	swap_chain->Release();
	device->Release();
	context->Release();
	renderTargetView->Release();

	depthState->Release();
	depthDisableState->Release();

	depthStencilView->Release();
	depthStencilBuffer->Release();
}


