#include "fontClass.h"

fontClass::fontClass()
{
}

fontClass::~fontClass()
{
}

bool fontClass::initialize(ID3D11Device *device, ID3D11DeviceContext *context)
{
	if (!load_data()) { 
		MessageBox(NULL, L" font fuck", L"font!!", MB_OK);
		return false; 
	}

	HRESULT result;
	result = CreateDDSTextureFromFile(device, L"font.dds", nullptr,  &m_texture, NULL);
	if (FAILED(result))
	{
		MessageBox(NULL, L" font fuck", L"font Image!!", MB_OK);
		return false;
	}

	if (!InitializeSentence( device, 16)) {
		MessageBox(NULL, L"What the fuck!", L"font!!", MB_OK);
		return false;
	}
	//UpdateSentence(context, m_sentence, "Trauma World!", 20, 20, 0.1f, 1.0f, 1.0f );

	initializeShader(device, context);

	return true;
}

void fontClass::clean()
{
	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	m_texture->Release();

	return;
}

bool fontClass::load_data()
{
	std::ifstream fin("fontdata.txt");

	int i;
	char ch;

	for (int index = 0; index < 95; index++) {
		fin >> i;
		fin.get(ch);
		fin.get(ch);
		fin.get(ch);

		fin >> m_font[index].left >> m_font[index].right >> m_font[index].size;
	}
	fin.close();
	return true;
}

bool fontClass::InitializeSentence(ID3D11Device* device, int maxLength)
{
	VertexType* vertices;
	unsigned long* indices;
	HRESULT result;
	int i;

	m_sentence = new SentenceType;
	if (!m_sentence)
	{
		return false;
	}

	m_sentence->vertexBuffer = 0;
	m_sentence->indexBuffer = 0;
	m_sentence->maxLength = maxLength;
	m_sentence->vertexCount = 6 * maxLength;
	m_sentence->indexCount = 6 * maxLength;

	vertices = new VertexType[6 * maxLength];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[6 * maxLength];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * 6 * maxLength));

	// Initialize the index array.
	for (i = 0; i < 6 * maxLength; i++)
	{
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_sentence->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_sentence->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_sentence->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_sentence->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	delete[] indices;

	return true;
}

bool fontClass::UpdateSentence(
	ID3D11DeviceContext* deviceContext,
	char* text, 
	int positionX, int positionY,
	float red, float green, float blue
	)
{
	HRESULT result;

	VertexType* vertices;
	VertexType* verticesPtr;
	int numLetters;
	float drawX, drawY;

	m_sentence->red = red, m_sentence->green = green, m_sentence->blue = blue;

	numLetters = (int)strlen(text);
	if (numLetters > m_sentence->maxLength)
	{
		return false;
	}

	vertices = new VertexType[m_sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * m_sentence->vertexCount));

	drawX = (float)(((1024 / 2) * -1) + positionX);
	drawY = (float)((768 / 2) - positionY);

	BuildVertexArray((void*)vertices, text, drawX, drawY);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_sentence->vertexCount));

	deviceContext->Unmap(m_sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}


bool fontClass::initializeShader(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	result = D3DCompileFromFile(L"font.vs", NULL, NULL, "FontVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(NULL, L"OOPS!", L"Missing Shader File", MB_OK);
		return false;
	}

	result = D3DCompileFromFile(L"font.ps", NULL, NULL, "FontPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		MessageBox(NULL, L"oops hmm", L"Missing Shader File", MB_OK);
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 
		NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	polygonLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release(); vertexShaderBuffer = 0;
	pixelShaderBuffer->Release(); pixelShaderBuffer = 0;

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool fontClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 pixelColor)
{
	HRESULT result;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_constantBuffer, 0);

	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);


	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (PixelBufferType*)mappedResource.pData;
	dataPtr2->pixelColor = pixelColor;

	deviceContext->Unmap(m_pixelBuffer, 0);

	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);
	deviceContext->PSSetShaderResources(0, 1, &m_texture);  //순서에 신경쓸 것! 매개변수 texture는 쓸 일이 없다.

	return true;
}

void fontClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	//deviceContext->IASetPrimitiveTopology는 큐브를 렌더하면서 생성되어 있다?!

	deviceContext->IASetInputLayout(m_layout);
	deviceContext->IASetVertexBuffers(0, 1, &(m_sentence->vertexBuffer), &stride, &offset);
	deviceContext->IASetIndexBuffer((m_sentence->indexBuffer), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void fontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	int numLetters, index, i, letter;
	VertexType* vertexPtr;

	vertexPtr = (VertexType*)vertices;
	numLetters = (int)strlen(sentence);

	index = 0;
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		if (letter == 0) {
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}

	return;
}



//WCHAR Buf[20], temp[20] = L"v : %s and i: %d";
//wsprintf(Buf, temp, sentence, letter);
//MessageBox(NULL, Buf, L"123!!", MB_OK);