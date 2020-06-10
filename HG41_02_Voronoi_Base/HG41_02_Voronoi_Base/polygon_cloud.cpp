
#include "main.h"
#include "renderer.h"
#include "shader.h"

#include "game_object.h"
#include "polygon_cloud.h"
#include "texture.h"
#include "camera.h"

#include "input.h"




void CPolygonCloud::Init()
{
	VERTEX_3D vertex[4];


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
	m_Shader->Init("shader2DCloudVS.cso", "shader2DCloudPS.cso");


}


void CPolygonCloud::Uninit()
{
	m_Shader->Uninit();
	delete m_Shader;

	m_VertexBuffer->Release();

}


void CPolygonCloud::Update()
{


}


void CPolygonCloud::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);


	XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, XMMatrixIdentity());

	m_Shader->SetWorldMatrix(&identity);
	m_Shader->SetViewMatrix(&identity);

	XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1000.0f));
	m_Shader->SetProjectionMatrix(&projection);

	CCamera* camera = CCamera::GetInstance();
	m_Shader->SetCameraPosition(&camera->GetPosition());
	XMFLOAT3 cameraRot = camera->GetRotation();
	static float time = 0.0f;
	XMFLOAT4 Parameter = XMFLOAT4(cameraRot.x, cameraRot.y, cameraRot.z, time);
	m_Shader->SetPrameter(Parameter);

	XMFLOAT4X4 rot;
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(cameraRot.x, cameraRot.y, cameraRot.z);
	XMStoreFloat4x4(&rot, mtxRot);
	m_Shader->SetWorldMatrix(&rot);

	time += 0.1f;

	m_Shader->Set();


	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);

}