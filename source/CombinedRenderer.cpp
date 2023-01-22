#include "pch.h"
#include "CombinedRenderer.h"
#include "Utils.h"

namespace dae
{
	CombinedRenderer::CombinedRenderer(SDL_Window* pWindow)
		:DxRenderer(pWindow, m_pDevice)
		,BaseRenderer(pWindow)
		,SoftwareRenderer(pWindow)
	{
		//Init Camera
		m_pCamera = new Camera{ Vector3{0.f, 0.f, 0.f}, 45.f, (static_cast<float>(m_Width) / static_cast<float>(m_Height)) };

		//textures
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_diffuse.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_normal.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_specular.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_gloss.png"));

		m_pExtraTexture.push_back(Texture::LoadFromFile(m_pDevice, "Resources/fireFX_diffuse.png"));
		
		//parse objects and put in corresponding mesh
		Utils::ParseOBJ("Resources/vehicle.obj", m_VehicleMesh.vertices, m_VehicleMesh.indices);
		Utils::ParseOBJ("Resources/fireFX.obj", m_CombustionMesh.vertices, m_CombustionMesh.indices);

		//Init Hardware and Software Rasterizer Renders
		DxRenderer::Initialize(m_pTextureMaps, m_pExtraTexture[0], m_pCamera, m_VehicleMesh, m_CombustionMesh);
		SoftwareRenderer::Initialize(m_pTextureMaps, m_pCamera, m_VehicleMesh);

		//check for current rasterizer version
		m_IsGpuRunning = true;
	}

	CombinedRenderer::~CombinedRenderer()
	{
		delete m_pCamera;
		for (Texture*& texture : m_pTextureMaps)
		{
			delete texture;
		}

		for (Texture*& texture : m_pExtraTexture)
		{
			delete texture;
		}
	}

	void CombinedRenderer::Render()
	{
		if (m_IsGpuRunning) DxRenderer::Render();
		
		else SoftwareRenderer::Render();
	}

	void CombinedRenderer::Update(Timer* pTimer)
	{
		DxRenderer::Update(pTimer);
		SoftwareRenderer::Update(pTimer);
	}


	//TOGGLE FUNCTIONS
	//----------------------------------------------------------------------------------

	void CombinedRenderer::SwitchProcces()
	{
		if (m_IsGpuRunning) m_IsGpuRunning = false;

		else m_IsGpuRunning = true;
	}


	void CombinedRenderer::ToggleVehicleRotation()
	{
		DxRenderer::ToggleRotation();
	}


	void CombinedRenderer::ToggleSWRenderOutput()
	{
		if (!m_IsGpuRunning) SoftwareRenderer::ToggleRenderOutput();
	}


	void CombinedRenderer::ToggleCullMode()
	{
		if (m_IsGpuRunning) DxRenderer::DxToggleCullMode();

		else SoftwareRenderer::SWToggleCullMode();
	}


	void CombinedRenderer::ToggleUniform()
	{
		if (m_IsGpuRunning) DxRenderer::DxToggleUniform();

		else SoftwareRenderer::ToggleUniformColor();
	}


	void CombinedRenderer::ToggleTechniquePass()
	{
		if (m_IsGpuRunning) DxRenderer::DxToggleTechniquePass();
	}


	void CombinedRenderer::ToggleCombustionRender()
	{
		if (m_IsGpuRunning) DxRenderer::DxToggleCombustionRender();
	}


	void CombinedRenderer::ToggleNormalMapSW()
	{
		if (!m_IsGpuRunning) SoftwareRenderer::ToggleNormalMap();
	}


	void CombinedRenderer::ToggleDepthVisualization()
	{
		if (!m_IsGpuRunning) SoftwareRenderer::ToggleColorOutput();
	}


	void CombinedRenderer::ToggleBoundingBoxesVisual()
	{
		if (!m_IsGpuRunning) SoftwareRenderer::ToggleBoundingBox();
	}
}
