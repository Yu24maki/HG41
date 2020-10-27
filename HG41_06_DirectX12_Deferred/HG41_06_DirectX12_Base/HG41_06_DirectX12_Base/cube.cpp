#include "main.h"
#include "cube.h"
#include "renderer.h"
#include "camera.h"


void CCube::Init()
{
	ComPtr<ID3D12Device> device = CRenderer::GetInstance()->GetDevice();

	HRESULT hr = {};

	D3D12_HEAP_PROPERTIES heapProperties = {};
	D3D12_RESOURCE_DESC resourceDesc = {};

	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;

	// 頂点バッファの作成
	resourceDesc.Width = sizeof(Vertex3D) * 36;
	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));
	assert(SUCCEEDED(hr));

	// 定数バッファの作成
	resourceDesc.Width = 256;		// 定数バッファは256バイトアライメント
	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBuffer));
	assert(SUCCEEDED(hr));

	// 頂点データ用意
	Vertex3D v[8];
	v[0].Position = { -0.5f,  0.5f,  0.5f };
	v[1].Position = { -0.5f,  0.5f, -0.5f };
	v[2].Position = {  0.5f,  0.5f, -0.5f };
	v[3].Position = {  0.5f,  0.5f,  0.5f };
	v[4].Position = { -0.5f, -0.5f,  0.5f };
	v[5].Position = { -0.5f, -0.5f, -0.5f };
	v[6].Position = {  0.5f, -0.5f, -0.5f };
	v[7].Position = {  0.5f, -0.5f,  0.5f };

	int index[36] = {
		0,3,1, 3,2,1,
		1,2,5, 2,6,5,
		2,3,6, 3,7,6,
		3,0,7, 0,4,7,
		0,1,4, 1,5,4,
		5,6,4, 6,7,4	
	};
	XMFLOAT3 normal[6] = {
		{ 0.0f, 1.0f, 0.0f},
		{ 0.0f, 0.0f,-1.0f},
		{ 1.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 1.0f},
		{-1.0f, 0.0f, 0.0f},
		{ 0.0f,-1.0f, 0.0f}
	};
	XMFLOAT2 texcoord[6] = {
		XMFLOAT2(0.0f,0.0f),
		XMFLOAT2(1.0f,0.0f),
		XMFLOAT2(0.0f,1.0f),
		XMFLOAT2(1.0f,0.0f),
		XMFLOAT2(1.0f,1.0f),
		XMFLOAT2(0.0f,1.0f),
	};

	// 頂点データの書き込み
	Vertex3D *buffer = {};
	hr = m_VertexBuffer->Map(0, nullptr, (void**)&buffer);
	assert(SUCCEEDED(hr));

	for (int i = 0; i < 36; i++)
	{
		buffer[i].Position = v[index[i]].Position;
		buffer[i].Normal = normal[i / 6];
		buffer[i].TexCoord = texcoord[i % 6];
	}


	m_VertexBuffer->Unmap(0, nullptr);


	// テクスチャ読み込み
	m_Texture.Load("asset/texture/field004.tga");
}


void CCube::Update()
{

}


void CCube::Draw(ID3D12GraphicsCommandList *pCommandList)
{
	HRESULT hr;

	static XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation.x += 0.01f;
	rotation.y += 0.01f;

	// マトリクス設定
	XMMATRIX view = XMLoadFloat4x4(&CCamera::GetView());
	XMMATRIX projection = XMLoadFloat4x4(&CCamera::GetProj());
	XMMATRIX world = XMMatrixScaling(2.0f,2.0f,2.0f) * XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// 定数バッファ設定
	Constant *constant = {};
	hr = m_ConstantBuffer->Map(0, nullptr, (void**)&constant);
	assert(SUCCEEDED(hr));

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world * view * projection));
	constant->WVP = matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));
	constant->World = matrix;
	XMFLOAT3 cpos = CCamera::GetPosition();
	constant->CameraPosition = XMFLOAT4(cpos.x, cpos.y, cpos.z, 0.0f);

	m_ConstantBuffer->Unmap(0, nullptr);
	pCommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->GetGPUVirtualAddress());

	// 頂点バッファ設定
	D3D12_VERTEX_BUFFER_VIEW vertexView = {};
	vertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(Vertex3D);
	vertexView.SizeInBytes = sizeof(Vertex3D) * 36;
	pCommandList->IASetVertexBuffers(0, 1, &vertexView);

	// テクスチャ設定
	ID3D12DescriptorHeap *dh[] = { *m_Texture.GetDescriptorHeap().GetAddressOf() };
	pCommandList->SetDescriptorHeaps(_countof(dh), dh);
	pCommandList->SetGraphicsRootDescriptorTable(1, m_Texture.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());


	// トポロジ設定
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// 描画
	pCommandList->DrawInstanced(36, 1, 0, 0);


}


void CCube::Uninit()
{

}
