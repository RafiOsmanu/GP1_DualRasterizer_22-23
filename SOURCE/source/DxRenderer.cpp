#include "pch.h"
#include "DxRenderer.h"


namespace dae 
{

	DxRenderer::DxRenderer(SDL_Window* pWindow, ID3D11Device*& pDevice)
		:BaseRenderer(pWindow)
		, m_pVehicleMesh{nullptr}
		, m_pCombustionMesh{nullptr}
		, m_IsUniform{false}
		, m_IsRotating{true}
		, m_IsCombustionRendering{true}
	{
		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//makes sure device u return has the value of the created device
		pDevice = m_pDevice;
		
	}

	DxRenderer::~DxRenderer()
	{
		m_pDevice->Release();
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();
		delete m_pMeshHandler;
		delete m_pMeshHandlerCombustions;
	}

	void DxRenderer::Update(const Timer* pTimer) 
	{
		m_pCamera->Update(pTimer);
		if (m_IsRotating)
		{
			m_pVehicleMesh->RotateMesh(pTimer);
			m_pVehicleMesh->Translate({ 0.f, 0.f, 50.f });
			m_pCombustionMesh->SetWorldMatrix(m_pVehicleMesh->worldMatrix);
		}
	}


	void DxRenderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. CLEAR RTV & DSV
		ColorRGB clearColor = ColorRGB{};

		if (m_IsUniform) clearColor = COLOR_UNIFORM;

		else clearColor = COLOR_HARDWARE;

		
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. Set PIPELINE + INVOKE DRAWCALLS (=RENDER)
		m_pMeshHandler->Render(m_pDeviceContext, m_pCamera, *m_pVehicleMesh);

		//for toggle of combustion
		if(m_IsCombustionRendering)
		m_pMeshHandlerCombustions->Render(m_pDeviceContext, m_pCamera, *m_pCombustionMesh);

		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);
	}

	void DxRenderer::DxToggleTechniquePass()
	{
		m_pMeshHandler->DxTogglePass(true);
		m_pMeshHandlerCombustions->DxTogglePass(false);
	}

	void DxRenderer::ToggleRotation()
	{
		if (m_IsRotating)
		{
			m_IsRotating = false;
			std::cout << "**(SHARED) Vehicle Rotation OFF\n";
		}

		else
		{
			m_IsRotating = true;
			std::cout << "**(SHARED) Vehicle Rotation ON\n";
		}
	}

	void DxRenderer::DxToggleCullMode()
	{
		m_pVehicleMaterial->ToggleCullMode();
	}

	void DxRenderer::DxToggleUniform()
	{
		if (m_IsUniform)
		{
			m_IsUniform = false;
			std::cout << "**(SHARED) Uniform ClearColor OFF\n";
		}

		else
		{
			m_IsUniform = true;
			std::cout << "**(SHARED) Uniform ClearColor ON\n";
		}
	}

	void DxRenderer::DxToggleCombustionRender()
	{
		if (m_IsCombustionRendering)
		{
			m_IsCombustionRendering = false;
			std::cout << "**(HARDWARE) FireFX OFF\n";
		}

		else
		{
			m_IsCombustionRendering = true;
			std::cout << "**(HARDWARE) FireFX ON\n";
		}
	}

	void DxRenderer::Initialize(std::vector<Texture*>& mainTextures, Texture* extraTexture, Camera* pCamera, Mesh& vehicleMesh, Mesh& combustionMesh)
	{
		m_pVehicleMesh = &vehicleMesh;
		m_pCombustionMesh = &combustionMesh;
		m_pCamera = pCamera;
		
		//Init material for vehicle mesh and handle the mesh
		m_pVehicleMaterial = new MaterialVehicle{ m_pDevice, m_pDeviceContext, L"Resources/PosCol3D.fx", mainTextures };
		m_pMeshHandler = new MeshHandler{ m_pDevice, vehicleMesh, m_pVehicleMaterial };

		//Init material for combustion mesh and handle the mesh
		m_pCombustionMat = new MaterialCombustion{ m_pDevice,L"Resources/Combustion.fx", extraTexture };
		m_pMeshHandlerCombustions = new MeshHandler{ m_pDevice, combustionMesh, m_pCombustionMat };
	}

	HRESULT DxRenderer::InitializeDirectX()
	{
		//1. Create Device & DeviceContext
		
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceflags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		//Create Device & DeviceContext
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceflags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if(FAILED(result))
		{ 
			return result;
		}

		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));

		if (FAILED(result))
		{
			return result;
		}

		///2. Create SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//Get The handle (HWND) from  the SDL backBuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		if (FAILED(result))
		{
			return result;
		}

		//3. Create DepthStencil(DS) & DepthStencilView (DSV)
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		//texture
		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);

		if (FAILED(result))
		{
			return result;
		}

		//Depth stencil View
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);

		if (FAILED(result))
		{
			return result;
		}

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			return result;
		}

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
		{
			return result;
		}

		//5. Bind RTV & DSV to output Merger State

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//6. Set Viewport

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		//Release hiddenLeak
		pDxgiFactory->Release();

		//Return result
		return result;
	}
}
