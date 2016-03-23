#include "shaderClass.h"

shaderClass::shaderClass()
{
}

shaderClass::~shaderClass()
{
}

bool shaderClass::initialize(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT hr;

	//Compile Shaders from shader file
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, 0);
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, 0);

	//Create the Shader Objects
	hr = device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//Create the Input Layout
	hr = device->CreateInputLayout(
		layout, numElements,
		VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(),
		&vertLayout);

	return true;
}

bool shaderClass::render(ID3D11Device *device, ID3D11DeviceContext *context)
{
	//Set Vertex and Pixel Shaders
	context->IASetInputLayout(vertLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(VS, 0, 0);
	context->PSSetShader(PS, 0, 0);
	return true;
}

void shaderClass::clean()
{
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();

	vertLayout->Release();
}
