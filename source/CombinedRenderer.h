#pragma once
#include "DxRenderer.h"
#include "SoftwareRenderer.h"
#include "Texture.h"

namespace dae
{
	class CombinedRenderer final : public DxRenderer, SoftwareRenderer
	{
	public:
		CombinedRenderer(SDL_Window* pWindow);
		~CombinedRenderer();

		void Render();
		void Update(Timer* pTimer);

		//Toggle Functions
		void SwitchProcces();
		void ToggleVehicleRotation();
		void ToggleSWRenderOutput();
		void ToggleCullMode();
		void ToggleUniform();
		void ToggleTechniquePass();
		void ToggleCombustionRender();
		void ToggleNormalMapSW();
		void ToggleDepthVisualization();
		void ToggleBoundingBoxesVisual();

	private:
		Mesh m_VehicleMesh;
		Mesh m_CombustionMesh;
		Camera* m_pCamera;

		std::vector<Texture*> m_pTextureMaps;

		ID3D11Device* m_pDevice;
		std::vector<Texture*> m_pExtraTexture;

		bool m_IsGpuRunning;
	};
}

