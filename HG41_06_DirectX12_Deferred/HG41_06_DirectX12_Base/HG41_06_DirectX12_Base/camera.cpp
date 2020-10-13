#include "camera.h"
#include "input.h"

XMFLOAT4X4 CCamera::m_mtxView;
XMFLOAT4X4 CCamera::m_mtxProj;

XMFLOAT3 CCamera::m_Position;
XMFLOAT3 CCamera::m_Rotation;
XMFLOAT3 CCamera::m_Front;
XMFLOAT3 CCamera::m_Up;
XMFLOAT3 CCamera::m_Right;

void CCamera::Init()
{
	XMMATRIX mtxI = XMMatrixIdentity();
	XMStoreFloat4x4(&m_mtxView, mtxI);
	XMStoreFloat4x4(&m_mtxProj, mtxI);

	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Front = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
}

void CCamera::Uninit()
{

}

void CCamera::Update()
{
	// ÉJÉÅÉâà⁄ìÆ
	{
		if (CInput::GetKeyPress('W'))
		{
			m_Position = (Vector3(m_Position) + Vector3(m_Front) * 0.1f).ToXMFLOAT3();
		}
		if (CInput::GetKeyPress('S'))
		{
			m_Position = (Vector3(m_Position) - Vector3(m_Front) * 0.1f).ToXMFLOAT3();
		}
		if (CInput::GetKeyPress('A'))
		{
			m_Position = (Vector3(m_Position) - Vector3(m_Right) * 0.1f).ToXMFLOAT3();
		}
		if (CInput::GetKeyPress('D'))
		{
			m_Position = (Vector3(m_Position) + Vector3(m_Right) * 0.1f).ToXMFLOAT3();
		}
		if (CInput::GetKeyPress('E'))
		{
			m_Position.y += 0.1f;
		}
		if (CInput::GetKeyPress('Q'))
		{
			m_Position.y -= 0.1f;
		}
	}

	// ÉJÉÅÉââÒì]
	{
		if (CInput::GetKeyPress(VK_LEFT))
		{
			m_Rotation.y -= 0.01f;
		}
		if (CInput::GetKeyPress(VK_RIGHT))
		{
			m_Rotation.y += 0.01f;
		}
		if (CInput::GetKeyPress(VK_UP))
		{
			m_Rotation.x -= 0.01f;
		}
		if (CInput::GetKeyPress(VK_DOWN))
		{
			m_Rotation.x += 0.01f;
		}
		XMVECTOR front;
		XMVECTOR up;
		XMVECTOR right;
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		front = rotation.r[2];
		up = rotation.r[1];
		right = rotation.r[0];
		XMStoreFloat3(&m_Front, front);
		XMStoreFloat3(&m_Up, up);
		XMStoreFloat3(&m_Right, right);
	}
}

void CCamera::Draw(ID3D12GraphicsCommandList *pCommandList)
{
	XMMATRIX view = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	view *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	view = XMMatrixInverse(nullptr, view);
	XMStoreFloat4x4(&m_mtxView, view);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 1000.0f);
	XMStoreFloat4x4(&m_mtxProj, proj);
}