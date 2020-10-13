#include "texture.h"
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void CTexture::Load(const char *pFilePath)
{
	CTexture texture;
	int width;
	int height;
	unsigned char *image;
	int bpp;
	int size;

	image = stbi_load(pFilePath, &width, &height, &bpp, STBI_rgb_alpha);

	size = width * height * bpp;

	ComPtr<ID3D12Device> device = CRenderer::GetInstance()->GetDevice();
	HRESULT hr{};

	// リソース作成
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
	assert(SUCCEEDED(hr));

	// デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
	assert(SUCCEEDED(hr));

	// シェーダリソースビュー作成
	D3D12_CPU_DESCRIPTOR_HANDLE handleSRV{};
	D3D12_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};

	resourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = 1;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;
	resourceViewDesc.Texture2D.PlaneSlice = 0;
	resourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	resourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	handleSRV = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateShaderResourceView(*m_Resource.GetAddressOf(), &resourceViewDesc, handleSRV);

	// 画像データ書き込み
	D3D12_BOX box = { 0, 0, 0, (UINT)width, (UINT)height, 1 };
	hr = m_Resource->WriteToSubresource(0, &box, &image[0], 4 * width, 4 * width * height);
	assert(SUCCEEDED(hr));

	stbi_image_free(image);
}