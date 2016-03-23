#include "MyObject.h"

MyObject::MyObject()
{
	m_VertBuffer = nullptr;
	m_IndexBuffer = nullptr;
}

MyObject::~MyObject()
{
}

bool MyObject::initialize(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT hr;

	rot = 0.0f;
	for (int index = 0; index < 27; index++) {
		permutation[index] = index;
	}

	Vertex v[] = {
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, (float)2 / 3),
		Vertex(1.0f,  1.0f, -1.0f, (float)1 / 3, (float)2 / 3),
		Vertex(1.0f, -1.0f, -1.0f, (float)1 / 3, 1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, 1.0f, 1.0f, 0.0f, (float)2 / 3),
		Vertex(1.0f,  1.0f, 1.0f, (float)1 / 3, (float)2 / 3),
		Vertex(1.0f,  -1.0f, 1.0f, (float)1 / 3, 1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, (float)2 / 3),
		Vertex(1.0f, 1.0f,  1.0f, (float)1 / 3, (float)2 / 3),
		Vertex(1.0f, 1.0f, -1.0f, (float)1 / 3, 1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f, 1.0f, 0.0f, (float)1 / 3),
		Vertex(1.0f, -1.0f,  1.0f, (float)1 / 3, (float)1 / 3),
		Vertex(1.0f, -1.0f,  -1.0f, (float)1 / 3, 0.0f),

		// Left Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, (float)2 / 3),
		Vertex(-1.0f,  1.0f, 1.0f, (float)1 / 3, (float)2 / 3),
		Vertex(-1.0f, -1.0f, 1.0f, (float)1 / 3, 1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, (float)2 / 3),
		Vertex(1.0f,  1.0f,  1.0f, (float)1 / 3, (float)2 / 3),
		Vertex(1.0f, -1.0f,  1.0f, (float)1 / 3, 1.0f),
	};

	for (int ix = 0; ix < 3; ix++) {
		for (int iy = 0; iy < 3; iy++) {
			for (int iz = 0; iz < 3; iz++) {
				for (int index = 0; index < 4; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] = 
						v[index] + Vertex((float)(-2 + ix*2), (float)(-2 + 2*iy), float(-2 + 2*iz), (float)1 / 3 * ix, -(float)1 / 3 * iy);
				}
				for (int index = 4; index < 8; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] =
						v[index] + Vertex((float)(-2 + ix * 2), (float)(-2 + 2 * iy), float(-2 + 2 * iz), (float)1 / 3 * ix, -(float)1 / 3 * iy);
				}
				for (int index = 8; index < 12; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] =
						v[index] + Vertex((float)(-2 + ix * 2), (float)(-2 + 2 * iy), float(-2 + 2 * iz), (float)1 / 3 * ix, -(float)1 / 3 * iz);
				}
				for (int index = 12; index < 16; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] =
						v[index] + Vertex((float)(-2 + ix * 2), (float)(-2 + 2 * iy), float(-2 + 2 * iz), (float)1 / 3 * ix, (float)1 / 3 * iz);
				}
				for (int index = 16; index < 20; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] =
						v[index] + Vertex((float)(-2 + ix * 2), (float)(-2 + 2 * iy), float(-2 + 2 * iz), (float)1 / 3 * iz, -(float)1 / 3 * iy);
				}
				for (int index = 20; index < 24; index++) {
					m_vertices[9 * ix + 3 * iy + iz][index] =
						v[index] + Vertex((float)(-2 + ix * 2), (float)(-2 + 2 * iy), float(-2 + 2 * iz), (float)1 / 3 * iz, -(float)1 / 3 * iy);
				}
			}
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 27 * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertex_data;
	ZeroMemory(&vertex_data, sizeof(vertex_data));
	vertex_data.pSysMem = m_vertices;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertex_data, &m_VertBuffer);

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	for (int ix = 0; ix < 3; ix++) {
		for (int iy = 0; iy < 3; iy++) {
			for (int iz = 0; iz < 3; iz++) {
				for (int index = 0; index < 36; index ++)
					m_indices[9*ix + 3*iy + iz][index] = indices[index] + 24 * (9*ix + 3*iy + iz);
			}
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 27 * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA index_data;
	ZeroMemory(&index_data, sizeof(index_data));
	index_data.pSysMem = m_indices;
	device->CreateBuffer(&indexBufferDesc, &index_data, &m_IndexBuffer);

	return true;
}

bool MyObject::render(ID3D11Device *device, ID3D11DeviceContext *context)
{
	//Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_VertBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	return true;
}

////// 우측면 동작들
void MyObject::rotation_right(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 18; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) - fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 18; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = -copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[24];
		permutation[24] = permutation[18];
		permutation[18] = permutation[20];
		permutation[20] = permutation[26];
		permutation[26] = temp;

		temp = permutation[21];
		permutation[21] = permutation[19];
		permutation[19] = permutation[23];
		permutation[23] = permutation[25];
		permutation[25] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::reverse_rotation_right(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 18; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) + fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = -fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 18; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = -copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[18];
		permutation[18] = permutation[24];
		permutation[24] = permutation[26];
		permutation[26] = permutation[20];
		permutation[20] = temp;

		temp = permutation[21];
		permutation[21] = permutation[25];
		permutation[25] = permutation[23];
		permutation[23] = permutation[19];
		permutation[19] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_rotation_X(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) - fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = -copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[24];
		permutation[24] = permutation[18];
		permutation[18] = permutation[20];
		permutation[20] = permutation[26];
		permutation[26] = temp;

		temp = permutation[21];
		permutation[21] = permutation[19];
		permutation[19] = permutation[23];
		permutation[23] = permutation[25];
		permutation[25] = temp;

		temp = permutation[15];
		permutation[15] = permutation[9];
		permutation[9] = permutation[11];
		permutation[11] = permutation[17];
		permutation[17] = temp;

		temp = permutation[16];
		permutation[16] = permutation[12];
		permutation[12] = permutation[10];
		permutation[10] = permutation[14];
		permutation[14] = temp;

		temp = permutation[6];
		permutation[6] = permutation[0];
		permutation[0] = permutation[2];
		permutation[2] = permutation[8];
		permutation[8] = temp;

		temp = permutation[3];
		permutation[3] = permutation[1];
		permutation[1] = permutation[5];
		permutation[5] = permutation[7];
		permutation[7] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}


////// 좌측면 동작
void MyObject::rotation_left(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 9; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) + fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = -fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 9; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = -copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[8];
		permutation[8] = permutation[2];
		permutation[2] = permutation[0];
		permutation[0] = temp;

		temp = permutation[7];
		permutation[7] = permutation[5];
		permutation[5] = permutation[1];
		permutation[1] = permutation[3];
		permutation[3] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::reverse_rotation_left(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 9; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) - fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 9; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = -copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[0];
		permutation[0] = permutation[2];
		permutation[2] = permutation[8];
		permutation[8] = temp;

		temp = permutation[7];
		permutation[7] = permutation[3];
		permutation[3] = permutation[1];
		permutation[1] = permutation[5];
		permutation[5] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_reverse_rotation_X(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fz = m_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) + fz * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.z = -fy * XMScalarSin(XM_PIDIV2 / 60) + fz * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = copy_vertices[permutation[index]][i].pos.z;
				m_vertices[permutation[index]][i].pos.z = -copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[24];
		permutation[24] = permutation[26];
		permutation[26] = permutation[20];
		permutation[20] = permutation[18];
		permutation[18] = temp;

		temp = permutation[21];
		permutation[21] = permutation[25];
		permutation[25] = permutation[23];
		permutation[23] = permutation[19];
		permutation[19] = temp;

		temp = permutation[15];
		permutation[15] = permutation[17];
		permutation[17] = permutation[11];
		permutation[11] = permutation[9];
		permutation[9] = temp;

		temp = permutation[16];
		permutation[16] = permutation[14];
		permutation[14] = permutation[10];
		permutation[10] = permutation[12];
		permutation[12] = temp;

		temp = permutation[6];
		permutation[6] = permutation[8];
		permutation[8] = permutation[2];
		permutation[2] = permutation[0];
		permutation[0] = temp;

		temp = permutation[3];
		permutation[3] = permutation[7];
		permutation[7] = permutation[5];
		permutation[5] = permutation[1];
		permutation[1] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}


////// 윗면 동작

void MyObject::rotation_up(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fz, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 6; index < 27; index++) {
			if ((index % 9) > 5) {
				for (int i = 0; i < 24; i++) {
					fz = m_vertices[permutation[index]][i].pos.z;
					fx = m_vertices[permutation[index]][i].pos.x;
					m_vertices[permutation[index]][i].pos.z = fz * XMScalarCos(XM_PIDIV2 / 60) - fx * XMScalarSin(XM_PIDIV2 / 60);
					m_vertices[permutation[index]][i].pos.x = fz * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
				}
			}
		}
	}
	else {
		for (int index = 6; index < 27; index++) {
			if ((index % 9) > 5) {
				for (int i = 0; i < 24; i++) {
					m_vertices[permutation[index]][i].pos.z = -copy_vertices[permutation[index]][i].pos.x;
					m_vertices[permutation[index]][i].pos.x = copy_vertices[permutation[index]][i].pos.z;
				}
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[24];
		permutation[24] = permutation[26];
		permutation[26] = permutation[8];
		permutation[8] = temp;

		temp = permutation[7];
		permutation[7] = permutation[15];
		permutation[15] = permutation[25];
		permutation[25] = permutation[17];
		permutation[17] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::reverse_rotation_up(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fz, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 6; index < 27; index++) {
			if ((index % 9) > 5) {
				for (int i = 0; i < 24; i++) {
					fz = m_vertices[permutation[index]][i].pos.z;
					fx = m_vertices[permutation[index]][i].pos.x;
					m_vertices[permutation[index]][i].pos.z = fz * XMScalarCos(XM_PIDIV2 / 60) + fx * XMScalarSin(XM_PIDIV2 / 60);
					m_vertices[permutation[index]][i].pos.x = -fz * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
				}
			}
		}
	}
	else {
		for (int index = 6; index < 27; index++) {
			if ((index % 9) > 5) {
				for (int i = 0; i < 24; i++) {
					m_vertices[permutation[index]][i].pos.z = copy_vertices[permutation[index]][i].pos.x;
					m_vertices[permutation[index]][i].pos.x = -copy_vertices[permutation[index]][i].pos.z;
				}
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[8];
		permutation[8] = permutation[26];
		permutation[26] = permutation[24];
		permutation[24] = temp;

		temp = permutation[7];
		permutation[7] = permutation[17];
		permutation[17] = permutation[25];
		permutation[25] = permutation[15];
		permutation[15] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_rotation_Y(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fx, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fz = m_vertices[permutation[index]][i].pos.z;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.z = fz * XMScalarCos(XM_PIDIV2 / 60) - fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = fz * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.z = -copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = copy_vertices[permutation[index]][i].pos.z;
			}
		}
		WORD temp = permutation[24];
		permutation[24] = permutation[26];
		permutation[26] = permutation[8];
		permutation[8] = permutation[6];
		permutation[6] = temp;

		temp = permutation[7];
		permutation[7] = permutation[15];
		permutation[15] = permutation[25];
		permutation[25] = permutation[17];
		permutation[17] = temp;

		temp = permutation[3];
		permutation[3] = permutation[21];
		permutation[21] = permutation[23];
		permutation[23] = permutation[5];
		permutation[5] = temp;

		temp = permutation[4];
		permutation[4] = permutation[12];
		permutation[12] = permutation[22];
		permutation[22] = permutation[14];
		permutation[14] = temp;

		temp = permutation[0];
		permutation[0] = permutation[18];
		permutation[18] = permutation[20];
		permutation[20] = permutation[2];
		permutation[2] = temp;

		temp = permutation[1];
		permutation[1] = permutation[9];
		permutation[9] = permutation[19];
		permutation[19] = permutation[11];
		permutation[11] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_reverse_rotation_Y(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fx, fz;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fz = m_vertices[permutation[index]][i].pos.z;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.z = fz * XMScalarCos(XM_PIDIV2 / 60) + fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = -fz * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.z = copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = -copy_vertices[permutation[index]][i].pos.z;
			}
		}
		WORD temp = permutation[24];
		permutation[24] = permutation[6];
		permutation[6] = permutation[8];
		permutation[8] = permutation[26];
		permutation[26] = temp;

		temp = permutation[7];
		permutation[7] = permutation[17];
		permutation[17] = permutation[25];
		permutation[25] = permutation[15];
		permutation[15] = temp;

		temp = permutation[3];
		permutation[3] = permutation[5];
		permutation[5] = permutation[23];
		permutation[23] = permutation[21];
		permutation[21] = temp;

		temp = permutation[4];
		permutation[4] = permutation[14];
		permutation[14] = permutation[22];
		permutation[22] = permutation[12];
		permutation[12] = temp;

		temp = permutation[0];
		permutation[0] = permutation[2];
		permutation[2] = permutation[20];
		permutation[20] = permutation[18];
		permutation[18] = temp;

		temp = permutation[1];
		permutation[1] = permutation[11];
		permutation[11] = permutation[19];
		permutation[19] = permutation[9];
		permutation[9] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}
////// 정면 동작들

void MyObject::rotation_front(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index += 3) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) - fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = fy * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index += 3) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = -copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[0];
		permutation[0] = permutation[18];
		permutation[18] = permutation[24];
		permutation[24] = temp;

		temp = permutation[15];
		permutation[15] = permutation[3];
		permutation[3] = permutation[9];
		permutation[9] = permutation[21];
		permutation[21] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::reverse_rotation_front(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index += 3) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) + fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = -fy * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index += 3) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = -copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[6];
		permutation[6] = permutation[24];
		permutation[24] = permutation[18];
		permutation[18] = permutation[0];
		permutation[0] = temp;

		temp = permutation[15];
		permutation[15] = permutation[21];
		permutation[21] = permutation[9];
		permutation[9] = permutation[3];
		permutation[3] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_rotation_Z(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) - fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = fy * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = -copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[0];
		permutation[0] = permutation[18];
		permutation[18] = permutation[24];
		permutation[24] = permutation[6];
		permutation[6] = temp;

		temp = permutation[3];
		permutation[3] = permutation[9];
		permutation[9] = permutation[21];
		permutation[21] = permutation[15];
		permutation[15] = temp;

		temp = permutation[7];
		permutation[7] = permutation[1];
		permutation[1] = permutation[19];
		permutation[19] = permutation[25];
		permutation[25] = temp;

		temp = permutation[4];
		permutation[4] = permutation[10];
		permutation[10] = permutation[22];
		permutation[22] = permutation[16];
		permutation[16] = temp;

		temp = permutation[2];
		permutation[2] = permutation[20];
		permutation[20] = permutation[26];
		permutation[26] = permutation[8];
		permutation[8] = temp;

		temp = permutation[5];
		permutation[5] = permutation[11];
		permutation[11] = permutation[23];
		permutation[23] = permutation[17];
		permutation[17] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::total_reverse_rotation_Z(ID3D11DeviceContext *context, bool *input_blocked)
{
	float fy, fx;

	if (rot == 0.0f) {
		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 24; j++) {
				copy_vertices[i][j].pos = m_vertices[i][j].pos;
				copy_vertices[i][j].texCoord = m_vertices[i][j].texCoord;
			}
		}
	}
	rot += XM_PIDIV2 / 60;

	if (rot < XM_PIDIV2) {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				fy = m_vertices[permutation[index]][i].pos.y;
				fx = m_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.y = fy * XMScalarCos(XM_PIDIV2 / 60) + fx * XMScalarSin(XM_PIDIV2 / 60);
				m_vertices[permutation[index]][i].pos.x = -fy * XMScalarSin(XM_PIDIV2 / 60) + fx * XMScalarCos(XM_PIDIV2 / 60);
			}
		}
	}
	else {
		for (int index = 0; index < 27; index++) {
			for (int i = 0; i < 24; i++) {
				m_vertices[permutation[index]][i].pos.y = copy_vertices[permutation[index]][i].pos.x;
				m_vertices[permutation[index]][i].pos.x = -copy_vertices[permutation[index]][i].pos.y;
			}
		}
		WORD temp = permutation[0];
		permutation[0] = permutation[6];
		permutation[6] = permutation[24];
		permutation[24] = permutation[18];
		permutation[18] = temp;

		temp = permutation[3];
		permutation[3] = permutation[15];
		permutation[15] = permutation[21];
		permutation[21] = permutation[9];
		permutation[9] = temp;

		temp = permutation[7];
		permutation[7] = permutation[25];
		permutation[25] = permutation[19];
		permutation[19] = permutation[1];
		permutation[1] = temp;

		temp = permutation[4];
		permutation[4] = permutation[16];
		permutation[16] = permutation[22];
		permutation[22] = permutation[10];
		permutation[10] = temp;

		temp = permutation[2];
		permutation[2] = permutation[8];
		permutation[8] = permutation[26];
		permutation[26] = permutation[20];
		permutation[20] = temp;

		temp = permutation[5];
		permutation[5] = permutation[17];
		permutation[17] = permutation[23];
		permutation[23] = permutation[11];
		permutation[11] = temp;
		rot = 0.0f;
		*input_blocked = false;
	}
	context->UpdateSubresource(m_VertBuffer, 0, NULL, m_vertices, 0, 0);
}

void MyObject::clean()
{
	m_VertBuffer->Release();
	m_IndexBuffer->Release();
}


