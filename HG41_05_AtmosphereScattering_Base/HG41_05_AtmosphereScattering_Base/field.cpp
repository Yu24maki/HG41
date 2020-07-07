
#include "main.h"
#include "renderer.h"
#include "game_object.h"
#include "field.h"
#include "texture.h"
#include "input.h"




void CField::Init()
{

	for (int z = 0; z < FIELD_Z; z++)
	{
		for (int x = 0; x < FIELD_X; x++)
		{
			m_Vertex[z * FIELD_X + x].Position.x = (x - FIELD_X / 2) * 20.0f;
			m_Vertex[z * FIELD_X + x].Position.z = (-z + FIELD_Z / 2) * 20.0f;
			m_Vertex[z * FIELD_X + x].Position.y = sinf(x*1.0f) * sinf(z*0.5f) * 20.0f;
			//m_Vertex[z * FIELD_X + x].Position.y = 0.0f;
			m_Vertex[z * FIELD_X + x].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			m_Vertex[z * FIELD_X + x].TexCoord = XMFLOAT2(x, z);
			m_Vertex[z * FIELD_X + x].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	for (int z = 1; z < FIELD_Z - 1; z++)
	{
		for (int x = 1; x < FIELD_X - 1; x++)
		{
			XMFLOAT3 va, vb, n;
			float len;

			va.x = m_Vertex[(z - 1) * FIELD_X + x].Position.x - m_Vertex[(z + 1) * FIELD_X + x].Position.x;
			va.y = m_Vertex[(z - 1) * FIELD_X + x].Position.y - m_Vertex[(z + 1) * FIELD_X + x].Position.y;
			va.z = m_Vertex[(z - 1) * FIELD_X + x].Position.z - m_Vertex[(z + 1) * FIELD_X + x].Position.z;

			vb.x = m_Vertex[z * FIELD_X + (x + 1)].Position.x - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.x;
			vb.y = m_Vertex[z * FIELD_X + (x + 1)].Position.y - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.y;
			vb.z = m_Vertex[z * FIELD_X + (x + 1)].Position.z - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.z;

			n.x = va.y * vb.z - va.z * vb.y;
			n.y = va.z * vb.x - va.x * vb.z;
			n.z = va.x * vb.y - va.y * vb.x;

			len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);

			n.x /= len;
			n.y /= len;
			n.z /= len;

			m_Vertex[z * FIELD_X + x].Normal = n;
		}
	}

	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * FIELD_X * FIELD_Z;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}


	unsigned short index[(FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2];

	unsigned short i = 0;
	for (int z = 0; z < FIELD_Z - 1; z++)
	{
		for (int x = 0; x < FIELD_X; x++)
		{
			index[i] = (z + 1) * FIELD_X + x;
			i++;
			index[i] = z * FIELD_X + x;
			i++;
		}

		if (z == FIELD_Z - 2)
			break;

		index[i] = z * FIELD_X + FIELD_X - 1;
		i++;
		index[i] = (z + 2) * FIELD_X;
		i++;
	}


	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short) * ((FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}




	m_Texture = new CTexture();
	m_Texture->Load("data/TEXTURE/field004.tga");
		


	m_Position = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	m_Rotation = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	m_Scale = XMFLOAT3( 1.0f, 1.0f, 1.0f );


}


void CField::Uninit()
{

	m_VertexBuffer->Release();


	m_Texture->Unload();
	delete m_Texture;

}


void CField::Update()
{

}


void CField::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof( VERTEX_3D );
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );

	// インデックスバッファ設定
	CRenderer::GetDeviceContext()->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );


	// テクスチャ設定
	CRenderer::SetTexture( m_Texture );

	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling( m_Scale.x, m_Scale.y, m_Scale.z );
	world *= XMMatrixRotationRollPitchYaw( m_Rotation.x, m_Rotation.y, m_Rotation.z );
	world *= XMMatrixTranslation( m_Position.x, m_Position.y, m_Position.z );
	CRenderer::SetWorldMatrix( &world );

	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );


	// ポリゴン描画
	CRenderer::GetDeviceContext()->DrawIndexed(((FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2), 0, 0 );

}




float CField::GetHeight(XMFLOAT3 Position)
{

	int x, z;
	XMFLOAT3 p0, p1, p2;

	x = Position.x + FIELD_X / 2;
	z = Position.x + FIELD_X / 2;

	p0 = m_Vertex[z * FIELD_X + x].Position;

/*
	for (int z = 0; z < FIELD_Z; z++)
	{
		for (int x = 0; x < FIELD_X; x++)
		{
			vertex[z * FIELD_X + x].Position.x = x - FIELD_X / 2;
			vertex[z * FIELD_X + x].Position.z = -z + FIELD_Z / 2;
			vertex[z * FIELD_X + x].Position.y = sinf(x * 0.5f) * sinf(z * 0.3f) * 2.0f;
			vertex[z * FIELD_X + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[z * FIELD_X + x].TexCoord = XMFLOAT2(x, z);
			vertex[z * FIELD_X + x].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}
*/

	return 0.0f;
}
