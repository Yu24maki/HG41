#pragma once




using namespace DirectX;
using namespace Microsoft::WRL;

struct Vertex3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

struct Constant
{
	XMFLOAT4X4 WVP;
	XMFLOAT4X4 World;
	XMFLOAT4 CameraPosition;
};

struct Material
{
	XMFLOAT4 Albedo;
	float Metallic;
	float Roughness;
	float dummy;
};


class CRenderer
{

private:

	static CRenderer*					m_Instance;


	HWND								m_WindowHandle;
	int									m_WindowWidth;
	int									m_WindowHeight;
	
	UINT64								m_Frame;
	UINT								m_RTIndex;

	ComPtr<IDXGIFactory4>				m_Factory;
	ComPtr<IDXGIAdapter3>				m_Adapter;
	ComPtr<ID3D12Device>				m_Device;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	HANDLE								m_FenceEvent;
	ComPtr<ID3D12Fence>					m_Fence;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12GraphicsCommandList>	m_GraphicsCommandList;
	ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	ComPtr<ID3D12Resource>				m_RenderTarget[2];
	ComPtr<ID3D12DescriptorHeap>		m_DescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RTHandle[2];
	ComPtr<ID3D12Resource>				m_DepthBuffer;
	ComPtr<ID3D12DescriptorHeap>		m_DHDS;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_DSHandle;
	ComPtr<ID3D12RootSignature>			m_RootSignature;

	D3D12_RECT							m_ScissorRect;
	D3D12_VIEWPORT						m_Viewport;

	ComPtr<ID3D12Resource>				m_NormalResource;
	ComPtr<ID3D12Resource>				m_DiffuseResource;
	ComPtr<ID3D12Resource>				m_PositionResource;
	ComPtr<ID3D12Resource>				m_DepthResource;

	ComPtr<ID3D12Resource>				m_EnvResource;
	ComPtr<ID3D12Resource>				m_IBLResource;

	ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RTHandleGeometry[4];
	ComPtr<ID3D12DescriptorHeap>		m_SRVDescriptorHeap;

	ComPtr<ID3D12PipelineState>			m_PipelineStateGeometry;
	ComPtr<ID3D12PipelineState>			m_PipelineStateLight;
	ComPtr<ID3D12PipelineState>			m_PipelineStateDebugGeometry;


public:


	CRenderer();

	void Initialize();
	void Update();
	void Draw();
	void SetResourceBarrier(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);
	void SetResourceBarrierGeometry(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);

	static CRenderer* GetInstance() { return m_Instance; }
	ComPtr<ID3D12Device> GetDevice() { return m_Device; }

};

