#pragma once

#include "main.h"
using namespace Microsoft::WRL;

struct ConstantCamera 
{
	XMFLOAT3 CameraPosition;
};

class CCamera
{
public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw(ID3D12GraphicsCommandList *pCommandList);

public:
	static XMFLOAT4X4 GetView() { return m_mtxView; }
	static XMFLOAT4X4 GetProj() { return m_mtxProj; }

	static XMFLOAT3 GetPosition() { return m_Position; }
	static XMFLOAT3 GetRotation() { return m_Rotation; }

	static void SetPosition(XMFLOAT3 position) { m_Position = position; }
	static void SetRotation(XMFLOAT3 rotation) { m_Rotation = rotation; }

private:
	static XMFLOAT4X4 m_mtxView;
	static XMFLOAT4X4 m_mtxProj;

	static XMFLOAT3 m_Position;
	static XMFLOAT3 m_Rotation;

	static XMFLOAT3 m_Front;
	static XMFLOAT3 m_Up;
	static XMFLOAT3 m_Right;

	static ComPtr<ID3D12Resource>	m_ConstantBuffer;

};