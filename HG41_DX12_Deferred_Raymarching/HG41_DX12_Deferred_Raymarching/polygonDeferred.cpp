
#include "main.h"
#include "renderer.h"
#include "polygon.h"




void CPolygonDeferred::Initialize()
{
	ComPtr<ID3D12Device> device = CRenderer::GetInstance()->GetDevice();


	HRESULT hr{};
	D3D12_HEAP_PROPERTIES heap_properties{};
	D3D12_RESOURCE_DESC   resource_desc{};
	
	heap_properties.Type					= D3D12_HEAP_TYPE_UPLOAD;
	heap_properties.CPUPageProperty			= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask		= 0;
	heap_properties.VisibleNodeMask			= 0;

	resource_desc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	resource_desc.Height				= 1;
	resource_desc.DepthOrArraySize		= 1;
	resource_desc.MipLevels				= 1;
	resource_desc.Format				= DXGI_FORMAT_UNKNOWN;
	resource_desc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resource_desc.SampleDesc.Count		= 1;
	resource_desc.SampleDesc.Quality	= 0;

	//���_�o�b�t�@�̍쐬
	resource_desc.Width = sizeof(Vertex3D) * 4;
	hr = device->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));
	assert(SUCCEEDED(hr));

	//�萔�o�b�t�@�̍쐬
	resource_desc.Width = sizeof(Constant);
	hr = device->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBuffer));
	assert(SUCCEEDED(hr));


	//���_�f�[�^�̏�������
	Vertex3D *buffer{};
	hr = m_VertexBuffer->Map(0, nullptr, (void**)&buffer);
	assert(SUCCEEDED(hr));

	buffer[0].Position = {0.0f,  0.0f,  0.0f};
	buffer[1].Position = { SCREEN_WIDTH,  0.0f, 0.0f};
	buffer[2].Position = {0.0f,  SCREEN_HEIGHT, 0.0f};
	buffer[3].Position = { SCREEN_WIDTH,  SCREEN_HEIGHT, 0.0f};
	buffer[0].Normal = {0.0f, 1.0f, 0.0f};
	buffer[1].Normal = {0.0f, 1.0f, 0.0f};
	buffer[2].Normal = {0.0f, 1.0f, 0.0f};
	buffer[3].Normal = {0.0f, 1.0f, 0.0f};
	buffer[0].TexCoord = {0.0f, 0.0f};
	buffer[1].TexCoord = {1.0f, 0.0f};
	buffer[2].TexCoord = {0.0f, 1.0f};
	buffer[3].TexCoord = {1.0f, 1.0f};

	m_VertexBuffer->Unmap(0, nullptr);


}


void CPolygonDeferred::Uninitialize()
{

}


void CPolygonDeferred::Update()
{

}


void CPolygonDeferred::Draw(ID3D12GraphicsCommandList *CommandList, ID3D12DescriptorHeap* Texture)
{
	HRESULT hr;


	//�}�g���N�X�ݒ�
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMMATRIX world = XMMatrixTranslation(0.0f, -2.0f, 0.0f);


	//�萔�o�b�t�@�ݒ�
	Constant *constant;
	hr = m_ConstantBuffer->Map(0, nullptr, (void**)&constant);
	assert(SUCCEEDED(hr));

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world * view * projection));
	constant->WVP = matrix;


	//XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));
	//constant->World = matrix;
	view = XMMatrixLookAtLH({ 0.0f, 0.0f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	projection = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 20.0f);
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(view * projection));
	constant->World = matrix;


	m_ConstantBuffer->Unmap(0, nullptr);

	CommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->GetGPUVirtualAddress());



	//���_�o�b�t�@�ݒ�
	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	vertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(Vertex3D);
	vertexView.SizeInBytes = sizeof(Vertex3D) * 4;
	CommandList->IASetVertexBuffers(0, 1, &vertexView);


	//�e�N�X�`���ݒ�
	ID3D12DescriptorHeap* dh[] = { Texture };
	CommandList->SetDescriptorHeaps(_countof(dh), dh);
	D3D12_GPU_DESCRIPTOR_HANDLE handle = Texture->GetGPUDescriptorHandleForHeapStart();
	CommandList->SetGraphicsRootDescriptorTable(1, handle);


	//�g�|���W�ݒ�
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//�`��
	CommandList->DrawInstanced(4, 1, 0, 0);

}
