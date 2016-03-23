#include "textureClass.h"

textureClass::textureClass()
{
}

textureClass::~textureClass()
{
}

bool textureClass::initialize(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT hr;

	hr = CreateWICTextureFromFile(device, L"trauma1.jpg", nullptr, &CubesTexture[0], NULL);
	hr = CreateWICTextureFromFile(device, L"trauma2.jpg", nullptr, &CubesTexture[1], NULL);
	hr = CreateWICTextureFromFile(device, L"trauma3.jpg", nullptr, &CubesTexture[2], NULL);
	hr = CreateWICTextureFromFile(device, L"trauma4.jpg", nullptr, &CubesTexture[3], NULL);
	hr = CreateWICTextureFromFile(device, L"trauma5.jpg", nullptr, &CubesTexture[4], NULL);
	hr = CreateWICTextureFromFile(device, L"trauma6.jpg", nullptr, &CubesTexture[5], NULL);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	///////////////**************new**************////////////////////
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	device->CreateBlendState(&blendDesc, &Transparency);

	//Create the Counter Clockwise and Clockwise Culling States
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = device->CreateRasterizerState(&cmdesc, &CWcullMode);
	///////////////**************new**************///////////////////
	return true;
}

bool textureClass::setBlend(ID3D11Device *device, ID3D11DeviceContext *context)
{
	float blendFactor[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	//Set the default blend state (no blending) for opaque objects
	//context->OMSetBlendState(0, 0, 0xffffffff);
	//Render opaque objects//
	//Set the blend state for transparent objects
	context->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	//Set the WVP matrix and send it to the constant buffer in effect file
	return true;
}

bool textureClass::clock_render(ID3D11Device *device, ID3D11DeviceContext *context, int i)
{
	context->PSSetShaderResources(0, 1, &CubesTexture[i]);
	context->PSSetSamplers(0, 1, &CubesTexSamplerState);
	///////////////**************new**************////////////////////
	context->RSSetState(CWcullMode);

	return true;
}

bool textureClass::counterclock_render(ID3D11Device *device, ID3D11DeviceContext *context, int i)
{
	context->PSSetShaderResources(0, 1, &CubesTexture[i]);
	context->PSSetSamplers(0, 1, &CubesTexSamplerState);
	///////////////**************new**************////////////////////
	context->RSSetState(CCWcullMode);

	return true;
}

void textureClass::clean()
{
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();

	for (int index = 0; index < 6; index++)
		CubesTexture[index]->Release();
	CubesTexSamplerState->Release();
}

void textureClass::change_image(ID3D11Device *device, WCHAR *filename, int number)
{ 
	HRESULT hr;

	hr = CreateWICTextureFromFile(device, filename, nullptr, &CubesTexture[number], NULL);
	if(FAILED(hr)){
		MessageBox(NULL, L"CHANGE2", L"CHANGE2", MB_OK);
	}
}
