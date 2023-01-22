#pragma once
#include "Camera.h"
#include "MaterialVehicle.h"
#include "MaterialCombustion.h"
#include "Texture.h"
#include "MeshHandler.h"
#include "BaseRenderer.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class DxRenderer : virtual public BaseRenderer
	{
	public:
		DxRenderer(SDL_Window* pWindow, ID3D11Device*& pDevice);
		~DxRenderer();

		virtual void Update(const Timer* pTimer) override;
		virtual void Render() const override;

		void DxToggleTechniquePass();
		void Initialize(std::vector<Texture*>& mainTextures, Texture* extraTexture, Camera* pCamera, Mesh& vehicleMesh, Mesh& combustionMesh);
		void ToggleRotation();
		void DxToggleCullMode();
		void DxToggleUniform();
		void DxToggleCombustionRender();
		

	private:

		
		bool m_IsInitialized{ false };

		//create device and DeviceContext
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		//create SwapChain
		IDXGISwapChain* m_pSwapChain{};

		//texture2D
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;

		//render targetBuffer
		ID3D11Resource* m_pRenderTargetBuffer;

		//render targetView
		ID3D11RenderTargetView* m_pRenderTargetView;
		

		MeshHandler* m_pMeshHandler;
		MeshHandler* m_pMeshHandlerCombustions;

		
		MaterialVehicle* m_pVehicleMaterial;
		MaterialCombustion* m_pCombustionMat;
		
		Camera* m_pCamera;
		Mesh* m_pVehicleMesh;
		Mesh* m_pCombustionMesh;


		//DIRECTX
		HRESULT InitializeDirectX();
		
		//functionallity
		bool m_IsRotating;
		bool m_IsUniform;
		bool m_IsCombustionRendering;
	};
}
