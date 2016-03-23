#include "processClass.h"

processClass::processClass()
{
}

processClass::~processClass()
{
}

bool processClass::initialize(int width, int height, ID3D11Device *device)
{
	HRESULT hr;
	m_width = width;
	m_height = height;

	camPosition = XMVectorSet(0.0f, 7.0f, -10.0f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//Set the View matrix
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	camProjection = XMMatrixPerspectiveFovLH(0.4f*XM_PI, (float)width / height, 1.0f, 1000.0f);
	ortho = XMMatrixOrthographicLH((float)width, (float)height, 0.1f, 1000.0f);

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	return true;
}

void processClass::clean()
{
	cbPerObjectBuffer->Release();
}

bool processClass::update(double time, double rot)
{
	cube1World = XMMatrixIdentity();

	camPosition = XMVectorSet(-10.0f * XMScalarSin(rot),  7.0f, -10.0f * XMScalarCos(rot) , 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	cube2World = XMMatrixIdentity();

	return true;
}

bool processClass::render(int choice, ID3D11Device *device, ID3D11DeviceContext *context)
{
	if (choice == 1) {
		cbPerObj.WVP = XMMatrixTranspose(cube1World * camView * camProjection);
	}
	else {
		cbPerObj.WVP = XMMatrixTranspose(cube2World * camView * camProjection);
	}
	context->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	context->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	return true;
}
