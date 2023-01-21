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

		m_pCamera = new Camera{ Vector3{0.f, 0.f, 0.f}, 45.f, (static_cast<float>(m_Width) / static_cast<float>(m_Height)) };

		//textures
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_diffuse.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_normal.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_specular.png"));
		m_pTextureMaps.push_back(Texture::LoadFromFile(m_pDevice, "Resources/vehicle_gloss.png"));

		m_pExtraTexture.push_back(Texture::LoadFromFile(m_pDevice, "Resources/fireFX_diffuse.png"));
		
		Utils::ParseOBJ("Resources/vehicle.obj", m_VehicleMesh.vertices, m_VehicleMesh.indices);
		Utils::ParseOBJ("Resources/fireFX.obj", m_CombustionMesh.vertices, m_CombustionMesh.indices);

		DxRenderer::Initialize(m_pTextureMaps, m_pExtraTexture[0], m_pCamera, m_VehicleMesh, m_CombustionMesh);
		SoftwareRenderer::Initialize(m_pTextureMaps, m_pCamera, m_VehicleMesh);

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
		if (m_IsGpuRunning) DxRenderer::Update(pTimer);
		
		else SoftwareRenderer::Update(pTimer);
	}

	void CombinedRenderer::SwitchProcces()
	{
		if (m_IsGpuRunning) m_IsGpuRunning = false;
		else m_IsGpuRunning = true;
	}
}
