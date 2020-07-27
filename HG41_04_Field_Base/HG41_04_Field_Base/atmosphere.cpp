
#include "main.h"
#include "renderer.h"
#include "game_object.h"
#include "shader.h"
#include "atmosphere.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "input.h"




void CAtmosphere::Init()
{

	float sizeXZ = 500.0f;
	float sizeY = 100.0f;

	for (int z = 0; z < SKY_Z; z++)
	{
		for (int x = 0; x < SKY_X; x++)
		{
			m_Vertex[z * SKY_X + x].Position.x = sinf((float)x / (SKY_X - 1) * XM_2PI) * (sinf((float)z / (SKY_Z - 1) * XM_PI)) * sizeXZ;
			m_Vertex[z * SKY_X + x].Position.z = cosf((float)x / (SKY_X - 1) * XM_2PI) * (sinf((float)z / (SKY_Z - 1) * XM_PI)) * sizeXZ;
			m_Vertex[z * SKY_X + x].Position.y = cosf((float)z / (SKY_Z - 1) * XM_PI) * sizeY;
			m_Vertex[z * SKY_X + x].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			m_Vertex[z * SKY_X + x].TexCoord = XMFLOAT2((float)x, (float)z);
			m_Vertex[z * SKY_X + x].Normal.x = m_Vertex[z * SKY_X + x].Position.x / sizeXZ;
			m_Vertex[z * SKY_X + x].Normal.y = m_Vertex[z * SKY_X + x].Position.y / sizeXZ;
			m_Vertex[z * SKY_X + x].Normal.z = m_Vertex[z * SKY_X + x].Position.z / sizeY;
		}
	}


	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * SKY_X * SKY_Z;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}


	unsigned short index[(SKY_X * 2 + 2) * (SKY_Z - 1) - 2];

	unsigned short i = 0;
	for (int z = 0; z < SKY_Z - 1; z++)
	{
		for (int x = 0; x < SKY_X; x++)
		{
			index[i] = (z + 1) * SKY_X + x;
			i++;
			index[i] = z * SKY_X + x;
			i++;
		}

		if (z == SKY_Z - 2)
			break;

		index[i] = z * SKY_X + SKY_X - 1;
		i++;
		index[i] = (z + 2) * SKY_X;
		i++;
	}


	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short) * ((SKY_X * 2 + 2) * (SKY_Z - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}

	m_Shader = new CShader();
	m_Shader->Init("shaderAtmosphereVS.cso", "shaderAtmospherePS.cso");



	m_Texture = new CTexture();
	m_Texture->Load("data/TEXTURE/field004.tga");



	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);


	m_LightRotation = 0.0f;
}


void CAtmosphere::Uninit()
{

	m_VertexBuffer->Release();


	m_Texture->Unload();
	delete m_Texture;

}


void CAtmosphere::Update()
{

}


void CAtmosphere::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	CRenderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	// テクスチャ設定
	CRenderer::SetTexture(m_Texture);

	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	world *= XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	world *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	XMFLOAT4X4 mtxWorld;
	XMStoreFloat4x4(&mtxWorld, world);

	m_Shader->SetWorldMatrix(&mtxWorld);

	CCamera *camera = CCamera::GetInstance();
	m_Shader->SetViewMatrix(&camera->GetViewMatrix());
	m_Shader->SetProjectionMatrix(&camera->GetProjectionMatrix());
	m_Shader->SetLight(CLight::GetInstance());

	m_Shader->Set();

	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	CRenderer::GetDeviceContext()->DrawIndexed(((SKY_X * 2 + 2) * (SKY_Z - 1) - 2), 0, 0);


}


