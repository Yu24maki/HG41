#include "main.h"
#include "field.h"
#include "renderer.h"
#include "camera.h"


void CField::Init()
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

	// ���_�o�b�t�@�̍쐬
	resourceDesc.Width = sizeof(Vertex3D) * 4;
	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@�̍쐬
	resourceDesc.Width = 256;		// �萔�o�b�t�@��256�o�C�g�A���C�����g
	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBuffer));
	assert(SUCCEEDED(hr));

	// ���_�f�[�^�̏�������
	Vertex3D *buffer = {};
	hr = m_VertexBuffer->Map(0, nullptr, (void**)&buffer);
	assert(SUCCEEDED(hr));

	buffer[0].Position = { -5.0f,  0.0f,  5.0f };
	buffer[1].Position = {  5.0f,  0.0f,  5.0f };
	buffer[2].Position = { -5.0f,  0.0f, -5.0f };
	buffer[3].Position = {  5.0f,  0.0f, -5.0f };
	buffer[0].Normal = { 0.0f,1.0f,0.0f };
	buffer[1].Normal = { 0.0f,1.0f,0.0f };
	buffer[2].Normal = { 0.0f,1.0f,0.0f };
	buffer[3].Normal = { 0.0f,1.0f,0.0f };
	buffer[0].TexCoord = { 0.0f,0.0f };
	buffer[1].TexCoord = { 10.0f,0.0f };
	buffer[2].TexCoord = { 0.0f,10.0f };
	buffer[3].TexCoord = { 10.0f,10.0f };

	m_VertexBuffer->Unmap(0, nullptr);


	// �e�N�X�`���ǂݍ���
	m_Texture.Load("asset/texture/field004.tga");
}


void CField::Update()
{

}


void CField::Draw(ID3D12GraphicsCommandList *pCommandList)
{
	HRESULT hr;

	// �}�g���N�X�ݒ�
	XMMATRIX view = XMLoadFloat4x4(&CCamera::GetView());
	XMMATRIX projection = XMLoadFloat4x4(&CCamera::GetProj());
	XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	// �萔�o�b�t�@�ݒ�
	Constant *constant = {};
	hr = m_ConstantBuffer->Map(0, nullptr, (void**)&constant);
	assert(SUCCEEDED(hr));

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world * view * projection));
	constant->WVP = matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));
	constant->World = matrix;

	m_ConstantBuffer->Unmap(0, nullptr);
	pCommandList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->GetGPUVirtualAddress());

	// ���_�o�b�t�@�ݒ�
	D3D12_VERTEX_BUFFER_VIEW vertexView = {};
	vertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(Vertex3D);
	vertexView.SizeInBytes = sizeof(Vertex3D) * 4;
	pCommandList->IASetVertexBuffers(0, 1, &vertexView);

	// �e�N�X�`���ݒ�
	ID3D12DescriptorHeap *dh[] = { *m_Texture.GetDescriptorHeap().GetAddressOf() };
	pCommandList->SetDescriptorHeaps(_countof(dh), dh);
	pCommandList->SetGraphicsRootDescriptorTable(1, m_Texture.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());


	// �g�|���W�ݒ�
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	// �`��
	pCommandList->DrawInstanced(4, 1, 0, 0);


}


void CField::Uninit()
{

}