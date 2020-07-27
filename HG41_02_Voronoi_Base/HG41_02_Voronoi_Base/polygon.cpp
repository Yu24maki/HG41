
#include "main.h"
#include "renderer.h"
#include "shader.h"

#include "game_object.h"
#include "polygon.h"
#include "texture.h"

#include "input.h"




void CPolygon::Init()
{
	VERTEX_3D vertex[4];


	//vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	//vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0.0f);
	//vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertex[1].TexCoord = XMFLOAT2(10.0f, 0.0f);

	//vertex[2].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);
	//vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertex[2].TexCoord = XMFLOAT2(0.0f, 10.0f);

	//vertex[3].Position = XMFLOAT3(200.0f, 200.0f, 0.0f);
	//vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertex[3].TexCoord = XMFLOAT2(10.0f, 10.0f);

	vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(-SCREEN_WIDTH, -SCREEN_HEIGHT);

	vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(SCREEN_WIDTH, -SCREEN_HEIGHT);

	vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(-SCREEN_WIDTH, SCREEN_HEIGHT);

	vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);


	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VERTEX_3D ) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer( &bd, &sd, &m_VertexBuffer );


	m_Shader = new CShader();
	m_Shader->Init( "shader2DTextureVS.cso", "shader2DTexturePS.cso" );
	
	m_Parameter = XMFLOAT4(0.0f, 0.0f, 0.002f, 0.0f);

}


void CPolygon::Uninit()
{
	m_Shader->Uninit();
	delete m_Shader;

	m_VertexBuffer->Release();

}


void CPolygon::Update()
{
	if (CInput::GetKeyPress('W'))
	{
		m_Parameter.y -= 0.1f * (m_Parameter.z * 500.0f);
	}
	if (CInput::GetKeyPress('A'))
	{
		m_Parameter.x -= 0.1f * (m_Parameter.z * 500.0f);
	}
	if (CInput::GetKeyPress('S'))
	{
		m_Parameter.y += 0.1f * (m_Parameter.z * 500.0f);
	}
	if (CInput::GetKeyPress('D'))
	{
		m_Parameter.x += 0.1f * (m_Parameter.z * 500.0f);
	}
	if (CInput::GetKeyPress('E'))
	{
		m_Parameter.z /= 1.1f;
	}
	if (CInput::GetKeyPress('Q'))
	{
		m_Parameter.z *= 1.1f;
	}

}


void CPolygon::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof( VERTEX_3D );
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );


	XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, XMMatrixIdentity());

	m_Shader->SetWorldMatrix(&identity);
	m_Shader->SetViewMatrix(&identity);
	m_Shader->SetPrameter(m_Parameter);

	XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
	m_Shader->SetProjectionMatrix(&projection);


	m_Shader->Set();


	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// ポリゴン描画
	CRenderer::GetDeviceContext()->Draw( 4, 0 );

}