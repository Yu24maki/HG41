
#include "main.h"
#include "renderer.h"
#include "shader.h"

#include "game_object.h"
#include "atmosphere.h"
#include "texture.h"

#include "input.h"




void CAtmosphere::Init()
{
	VERTEX_3D vertex[4];
	/*
		vertex[0].Position = XMFLOAT3( 100.0f, 100.0f, 0.0f );
		vertex[0].Diffuse  = XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );

		vertex[1].Position = XMFLOAT3( 300.0f, 100.0f, 0.0f );
		vertex[1].Diffuse  = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

		vertex[2].Position = XMFLOAT3( 100.0f, 300.0f, 0.0f );
		vertex[2].Diffuse  = XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

		vertex[3].Position = XMFLOAT3( 300.0f, 300.0f, 0.0f );
		vertex[3].Diffuse  = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	*/

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 999.9f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0.0f, 999.9f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 999.9f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 999.9f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);



	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);


	m_Shader = new CShader();
	m_Shader->Init("shaderAtmosphereVS.cso", "shaderAtmospherePS.cso");


	m_Texture[0] = new CTexture();
	m_Texture[0]->Load("data/TEXTURE/field004.tga");

	//m_Texture[1] = new CTexture();
	//m_Texture[1]->Load("data/TEXTURE/field004.tga");


	m_Blend = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_LightRotation = 0.0f;

}


void CAtmosphere::Uninit()
{
	m_Shader->Uninit();
	delete m_Shader;

	m_Texture[0]->Unload();
	delete m_Texture[0];

	//m_Texture[1]->Unload();
	//delete m_Texture[1];

	m_VertexBuffer->Release();

}


void CAtmosphere::Update()
{

	if (CInput::GetKeyPress(VK_UP))
		m_Blend.x += 0.01f;
	if (CInput::GetKeyPress(VK_DOWN))
		m_Blend.x -= 0.01f;

	//m_LightRotation += 0.01f;
	if (CInput::GetKeyPress('W'))
		m_LightRotation += 0.01f;
	if (CInput::GetKeyPress('S'))
		m_LightRotation -= 0.01f;


	LIGHT light;
	light.Direction = XMFLOAT4(0.0f, -cosf(m_LightRotation), sinf(m_LightRotation), 0.0f);
	light.Diffuse = COLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Ambient = COLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_Shader->SetLight(light);

}


void CAtmosphere::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);


	XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, XMMatrixIdentity());

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(m_LightRotation, 0.0f, 0.0f);
	XMFLOAT4X4 mrot;
	XMStoreFloat4x4(&mrot, mtxRot);

	m_Shader->SetWorldMatrix(&mrot);
	m_Shader->SetViewMatrix(&identity);

	XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1000.0f));
	m_Shader->SetProjectionMatrix(&projection);

	m_Shader->SetPrameter(m_Blend);

	m_Shader->Set();


	// テクスチャ設定
	CRenderer::SetTexture(m_Texture[0], 0);
	//CRenderer::SetTexture( m_Texture[1], 1 );
	//CRenderer::SetDepthTexture(1);

	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);

}