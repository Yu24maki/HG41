#pragma once

#include "main.h"

using namespace Microsoft::WRL;

class CPolygon2D
{
private:
	ComPtr<ID3D12Resource> m_VertexBuffer;
	ComPtr<ID3D12Resource> m_ConstantBuffer;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw(ID3D12GraphicsCommandList *pCommandList);

};