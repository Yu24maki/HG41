
#include "main.h"
#include "renderer.h"
#include "game_object.h"
#include "camera.h"
#include "input.h"





void CCamera::Init()
{

	m_Position = XMFLOAT3( 0.0f, 5.0f, -10.0f );
	m_Rotation = XMFLOAT3( -0.2f, 0.0f, 0.0f );


	m_Viewport.left = 0;
	m_Viewport.top = 0;
	m_Viewport.right = SCREEN_WIDTH;
	m_Viewport.bottom = SCREEN_HEIGHT;

}


void CCamera::Uninit()
{


}


void CCamera::Update()
{
	if (CInput::GetKeyPress(VK_LEFT))
		m_Rotation.y -= 0.01f;
	if (CInput::GetKeyPress(VK_RIGHT))
		m_Rotation.y += 0.01f;
	if (CInput::GetKeyPress(VK_UP))
		m_Rotation.x -= 0.01f;
	if (CInput::GetKeyPress(VK_DOWN))
		m_Rotation.x += 0.01f;


}



void CCamera::Draw()
{

	// ビューポート設定
	D3D11_VIEWPORT dxViewport;
	dxViewport.Width = (float)(m_Viewport.right - m_Viewport.left);
	dxViewport.Height = (float)(m_Viewport.bottom - m_Viewport.top);
	dxViewport.MinDepth = 0.0f;
	dxViewport.MaxDepth = 1.0f;
	dxViewport.TopLeftX = (float)m_Viewport.left;
	dxViewport.TopLeftY = (float)m_Viewport.top;

	CRenderer::GetDeviceContext()->RSSetViewports(1, &dxViewport);



	XMMATRIX	m_ViewMatrix;
	XMMATRIX	m_InvViewMatrix;
	XMMATRIX	m_ProjectionMatrix;


	// ビューマトリクス設定
	m_InvViewMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	m_InvViewMatrix *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	XMVECTOR det;
	m_ViewMatrix = XMMatrixInverse(&det, m_InvViewMatrix);

	CRenderer::SetViewMatrix(&m_ViewMatrix);



	// プロジェクションマトリクス設定
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(1.0f, dxViewport.Width / dxViewport.Height, 0.1f, 1000.0f);

	CRenderer::SetProjectionMatrix(&m_ProjectionMatrix);



}

