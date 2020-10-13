
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "polygon2D.h"
#include "field.h"
#include "cube.h"
#include "polygon_deferred.h"

CRenderer* CRenderer::m_Instance = nullptr;
CPolygon2D m_Polygon;
CField m_Field;
CCube m_Cube;
CPolygonDeferred m_PolygonDeferred;


CRenderer::CRenderer()
{
	assert(!m_Instance);
	m_Instance = this;


	m_WindowHandle = GetWindow();
	m_WindowWidth = SCREEN_WIDTH;
	m_WindowHeight = SCREEN_HEIGHT;
	m_Frame = 0;
	m_RTIndex = 0;



	m_Viewport.TopLeftX = 0.f; 
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width    = (FLOAT)m_WindowWidth;
	m_Viewport.Height   = (FLOAT)m_WindowHeight;
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;

	m_ScissorRect.top    = 0;
	m_ScissorRect.left   = 0;
	m_ScissorRect.right  = m_WindowWidth;
	m_ScissorRect.bottom = m_WindowHeight;




	Initialize();

}



void CRenderer::Initialize()
{
	HRESULT hr;


	//�f�o�C�X����
	{
		UINT flag{};
		hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		assert(SUCCEEDED(hr));

		hr = m_Factory->EnumAdapters(0, (IDXGIAdapter**)m_Adapter.GetAddressOf());
		assert(SUCCEEDED(hr));

		hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_Device));
		assert(SUCCEEDED(hr));
	}





	//�R�}���h�L���[����
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = 0;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		hr = m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()));
		assert(SUCCEEDED(hr));

		m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		assert(m_FenceEvent);

		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
		assert(SUCCEEDED(hr));
	}




	//�X���b�v�`�F�[������
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		ComPtr<IDXGISwapChain> swapChain{};

		swapChainDesc.BufferDesc.Width = m_WindowWidth;
		swapChainDesc.BufferDesc.Height = m_WindowHeight;
		swapChainDesc.OutputWindow = m_WindowHandle;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;


		hr = m_Factory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, swapChain.GetAddressOf());
		assert(SUCCEEDED(hr));

		hr = swapChain.As(&m_SwapChain);
		assert(SUCCEEDED(hr));

		m_RTIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}




	//�����_�[�^�[�Q�b�g����
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};

		heapDesc.NumDescriptors = 2;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;
		hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_DescriptorHeap.GetAddressOf()));
		assert(SUCCEEDED(hr));

		UINT size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (UINT i = 0; i < 2; ++i)
		{
			hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTarget[i]));
			assert(SUCCEEDED(hr));

			m_RTHandle[i] = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_RTHandle[i].ptr += size * i;
			m_Device->CreateRenderTargetView(m_RenderTarget[i].Get(), nullptr, m_RTHandle[i]);
		}
	}




	//�f�v�X�E�X�e���V���o�b�t�@����
	{
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.NumDescriptors = 1;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;
		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DHDS));
		assert(SUCCEEDED(hr));



		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_RESOURCE_DESC resourceDesc{};
		D3D12_CLEAR_VALUE clearValue{};

		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Width = m_WindowWidth;
		resourceDesc.Height = m_WindowHeight;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 0;
		resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		hr = m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_DepthBuffer));
		assert(SUCCEEDED(hr));



		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_DSHandle = m_DHDS->GetCPUDescriptorHandleForHeapStart();

		m_Device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsvDesc, m_DSHandle);
	}


	//�W�I���g���o�b�t�@����
	{
		//���\�[�X����
		{
			D3D12_HEAP_PROPERTIES heapProperties{};
			heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CreationNodeMask = 0;
			heapProperties.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC resourceDesc{};
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resourceDesc.Width = m_WindowWidth;
			resourceDesc.Height = m_WindowHeight;
			resourceDesc.DepthOrArraySize = 1;
			resourceDesc.MipLevels = 1;
			resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resourceDesc.SampleDesc.Count = 1;
			resourceDesc.SampleDesc.Quality = 0;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			resourceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

			D3D12_CLEAR_VALUE clearValue{};
			clearValue.Color[0] = 0.0f;
			clearValue.Color[1] = 0.0f;
			clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 1.0f;
			clearValue.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;


			hr = m_Device->CreateCommittedResource(&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				&clearValue,
				IID_PPV_ARGS(&m_NormalResource));
			assert(SUCCEEDED(hr));

			hr = m_Device->CreateCommittedResource(&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				&clearValue,
				IID_PPV_ARGS(&m_DiffuseResource));
			assert(SUCCEEDED(hr));
		}




		{
			//RTV�f�X�N���v�^�q�[�v
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NumDescriptors = 2;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NodeMask = 0;

			m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RTVDescriptorHeap));



			//RTV
			unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			D3D12_RENDER_TARGET_VIEW_DESC rtDesc = {};
			rtDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			rtDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;
			rtDesc.Texture2D.MipSlice = 0;
			rtDesc.Texture2D.PlaneSlice = 0;

			m_Device->CreateRenderTargetView(m_NormalResource.Get(), &rtDesc, handle);
			m_RTHandleGeometry[0] = handle;

			handle.ptr += (size);

			m_Device->CreateRenderTargetView(m_DiffuseResource.Get(), &rtDesc, handle);
			m_RTHandleGeometry[1] = handle;
		}




		{
			//SRV�f�X�N���v�^�q�[�v
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NumDescriptors = 2;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NodeMask = 0;

			m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_SRVDescriptorHeap));




			//SRV
			unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.PlaneSlice = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			m_Device->CreateShaderResourceView(m_NormalResource.Get(), &srvDesc, handle);
			handle.ptr += (size);

			m_Device->CreateShaderResourceView(m_DiffuseResource.Get(), &srvDesc, handle);

		}

	}


	//�R�}���h���X�g����
	{
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_GraphicsCommandList));
		assert(SUCCEEDED(hr));
	}


	//�V�O�l�`������
	{
		D3D12_DESCRIPTOR_RANGE		range[1]{};
		D3D12_ROOT_PARAMETER		root_parameters[2]{};
		D3D12_ROOT_SIGNATURE_DESC	root_signature_desc{};
		D3D12_STATIC_SAMPLER_DESC	sampler_desc{};
		ComPtr<ID3DBlob> blob{};


		root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		root_parameters[0].Descriptor.ShaderRegister = 0;
		root_parameters[0].Descriptor.RegisterSpace = 0;



		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		root_parameters[1].DescriptorTable.NumDescriptorRanges = 1;
		root_parameters[1].DescriptorTable.pDescriptorRanges = &range[0];


		//�T���v���[
		sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc.ShaderRegister = 0;
		sampler_desc.RegisterSpace = 0;
		sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		root_signature_desc.NumParameters = _countof(root_parameters);
		root_signature_desc.pParameters = root_parameters;
		root_signature_desc.NumStaticSamplers = 1;
		root_signature_desc.pStaticSamplers = &sampler_desc;

		hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		assert(SUCCEEDED(hr));
	}







	//�`��p�C�v���C������
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc{};
		

		std::vector<char> vertexShader;
		std::vector<char> pixelShader;


		//���_�V�F�[�_�[�ǂݍ���
		{
			std::ifstream file("vertexShader.cso", std::ios_base::in | std::ios_base::binary);
			assert(file);

			file.seekg(0, std::ios_base::end);
			int filesize = (int)file.tellg();
			file.seekg(0, std::ios_base::beg);

			vertexShader.resize(filesize);
			file.read(&vertexShader[0], filesize);

			file.close();


			pipeline_state_desc.VS.pShaderBytecode = &vertexShader[0];
			pipeline_state_desc.VS.BytecodeLength = filesize;
		}


		//�s�N�Z���V�F�[�_�[�ǂݍ���
		{
			std::ifstream file("pixelShader.cso", std::ios_base::in | std::ios_base::binary);
			assert(file);

			file.seekg(0, std::ios_base::end);
			int filesize = (int)file.tellg();
			file.seekg(0, std::ios_base::beg);

			pixelShader.resize(filesize);
			file.read(&pixelShader[0], filesize);

			file.close();


			pipeline_state_desc.PS.pShaderBytecode = &pixelShader[0];
			pipeline_state_desc.PS.BytecodeLength = filesize;
		}


		//�C���v�b�g���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		pipeline_state_desc.InputLayout.pInputElementDescs = InputElementDesc;
		pipeline_state_desc.InputLayout.NumElements = _countof(InputElementDesc);



		pipeline_state_desc.SampleDesc.Count = 1;
		pipeline_state_desc.SampleDesc.Quality = 0;
		pipeline_state_desc.SampleMask = UINT_MAX;

		pipeline_state_desc.NumRenderTargets = 1;
		pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;

		pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		pipeline_state_desc.pRootSignature = m_RootSignature.Get();


		//���X�^���C�U�X�e�[�g
		pipeline_state_desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipeline_state_desc.RasterizerState.FrontCounterClockwise = FALSE;
		pipeline_state_desc.RasterizerState.DepthBias = 0;
		pipeline_state_desc.RasterizerState.DepthBiasClamp = 0;
		pipeline_state_desc.RasterizerState.SlopeScaledDepthBias = 0;
		pipeline_state_desc.RasterizerState.DepthClipEnable = TRUE;
		pipeline_state_desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		pipeline_state_desc.RasterizerState.AntialiasedLineEnable = FALSE;
		pipeline_state_desc.RasterizerState.MultisampleEnable = FALSE;


		//�u�����h�X�e�[�g
		for (int i = 0; i < _countof(pipeline_state_desc.BlendState.RenderTarget); ++i)
		{
			pipeline_state_desc.BlendState.RenderTarget[i].BlendEnable = FALSE;
			pipeline_state_desc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			pipeline_state_desc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
			pipeline_state_desc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			pipeline_state_desc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			pipeline_state_desc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			pipeline_state_desc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			pipeline_state_desc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			pipeline_state_desc.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
			pipeline_state_desc.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_CLEAR;
		}
		pipeline_state_desc.BlendState.AlphaToCoverageEnable = FALSE;
		pipeline_state_desc.BlendState.IndependentBlendEnable = FALSE;


		//�f�v�X�E�X�e���V���X�e�[�g
		pipeline_state_desc.DepthStencilState.DepthEnable = TRUE;
		pipeline_state_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_state_desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipeline_state_desc.DepthStencilState.StencilEnable = FALSE;
		pipeline_state_desc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		pipeline_state_desc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

		pipeline_state_desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pipeline_state_desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipeline_state_desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pipeline_state_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


		hr = m_Device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&m_PipelineState));
		assert(SUCCEEDED(hr));
	}

	CCamera::Init();
	CCamera::SetPosition(XMFLOAT3(0.0f, 5.0f, -10.0f));
	CCamera::SetRotation(XMFLOAT3(0.5f, 0.0f, 0.0f));

	m_Polygon.Init();

	m_Field.Init();

	m_Cube.Init();

	m_PolygonDeferred.Init();
}


void CRenderer::Update()
{
	CCamera::Update();
	m_Polygon.Update();
	m_Field.Update();
	m_Cube.Update();

}



void CRenderer::Draw()
{
	HRESULT hr;


	FLOAT clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	////�����_�[�^�[�Q�b�g�p���\�[�X�o���A
	//SetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	////�f�v�X�o�b�t�@�E�����_�[�^�[�Q�b�g�̃N���A
	//m_GraphicsCommandList->ClearDepthStencilView(m_DSHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//m_GraphicsCommandList->ClearRenderTargetView(m_RTHandle[m_RTIndex], clearColor, 0, nullptr);

	
	// �W�I���g���p�X
	{
		// ���\�[�X�̏�Ԃ��V�F�[�_���\�[�X�p���烌���_�[�^�[�Q�b�g�p�ɕύX
		SetResourceBarrierGeometry(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// �[�x�o�b�t�@�ƃ����_�[�^�[�Q�b�g�̃N���A
		m_GraphicsCommandList->ClearDepthStencilView(m_DSHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_RTHandleGeometry[0], clearColor, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_RTHandleGeometry[1], clearColor, 0, nullptr);


		// ���[�g�V�O�l�`����PSO�̐ݒ�
		m_GraphicsCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
		m_GraphicsCommandList->SetPipelineState(m_PipelineState.Get());

		//�r���[�|�[�g�ƃV�U�[��`�̐ݒ�
		m_GraphicsCommandList->RSSetViewports(1, &m_Viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);



		//�����_�[�^�[�Q�b�g�̐ݒ�
		m_GraphicsCommandList->OMSetRenderTargets(2, m_RTHandleGeometry, TRUE, &m_DSHandle);



		//�I�u�W�F�N�g�`��

		CCamera::Draw(m_GraphicsCommandList.Get());
		m_Field.Draw(m_GraphicsCommandList.Get());
		m_Cube.Draw(m_GraphicsCommandList.Get());
		m_Polygon.Draw(m_GraphicsCommandList.Get());


		SetResourceBarrierGeometry(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	}

	// ���C�g�p�X
	{
		// ���\�[�X�̏�Ԃ��v���[���g�p���烌���_�[�^�[�Q�b�g�p�ɕύX
		SetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// �[�x�o�b�t�@�ƃ����_�[�^�[�Q�b�g�̃N���A
		m_GraphicsCommandList->ClearDepthStencilView(m_DSHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_RTHandle[m_RTIndex], clearColor, 0, nullptr);

		//�����_�[�^�[�Q�b�g�̐ݒ�
		m_GraphicsCommandList->OMSetRenderTargets(1, &m_RTHandle[m_RTIndex], TRUE, &m_DSHandle);

		// ������2D�|���S���`��
		m_PolygonDeferred.Draw(m_GraphicsCommandList.Get(), m_SRVDescriptorHeap.Get());


		//�v���[���g�p���\�[�X�o���A
		SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}

	hr = m_GraphicsCommandList->Close();
	assert(SUCCEEDED(hr));




	//�R�}���h���X�g���s
	ID3D12CommandList *const command_lists = m_GraphicsCommandList.Get();
	m_CommandQueue->ExecuteCommandLists(1, &command_lists);





	//���s�����R�}���h�̏I���҂�
	const UINT64 fence = m_Frame;
	hr = m_CommandQueue->Signal(m_Fence.Get(), fence);
	assert(SUCCEEDED(hr));
	m_Frame++;

	if (m_Fence->GetCompletedValue() < fence)
	{
		hr = m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		assert(SUCCEEDED(hr));

		WaitForSingleObject(m_FenceEvent, INFINITE);
	}





	hr = m_CommandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));

	hr = m_SwapChain->Present(1, 0);
	assert(SUCCEEDED(hr));

	//�J�����g�̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾����
	m_RTIndex = m_SwapChain->GetCurrentBackBufferIndex();

}





void CRenderer::SetResourceBarrier(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
{
	D3D12_RESOURCE_BARRIER resourceBarrier{};
	
	resourceBarrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource   = m_RenderTarget[m_RTIndex].Get();
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	resourceBarrier.Transition.StateBefore = BeforeState;
	resourceBarrier.Transition.StateAfter  = AfterState;

	m_GraphicsCommandList->ResourceBarrier(1, &resourceBarrier);

}


void CRenderer::SetResourceBarrierGeometry(D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
{
	D3D12_RESOURCE_BARRIER resourceBarrier[2] = {};

	resourceBarrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier[0].Transition.pResource = m_NormalResource.Get();
	resourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	resourceBarrier[0].Transition.StateBefore = BeforeState;
	resourceBarrier[0].Transition.StateAfter = AfterState;

	resourceBarrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier[1].Transition.pResource = m_DiffuseResource.Get();
	resourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	resourceBarrier[1].Transition.StateBefore = BeforeState;
	resourceBarrier[1].Transition.StateAfter = AfterState;

	m_GraphicsCommandList->ResourceBarrier(2, resourceBarrier);

}

