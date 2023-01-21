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
		void Render() const;

		void ToggleTechniquePass();
		void Initialize(std::vector<Texture*>& mainTextures, Texture* extraTexture, Camera* pCamera, const Mesh& vehicleMesh, const Mesh& combustionMesh);

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
		Mesh m_VehicleMesh;
		Mesh m_CombustionMesh;


		//DIRECTX
		HRESULT InitializeDirectX();
		//...
	};
}
