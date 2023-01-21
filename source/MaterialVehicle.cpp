#include "pch.h"
#include "MaterialVehicle.h"
#include "Texture.h"
#include "DataTypes.h"


namespace dae
{
	MaterialVehicle::MaterialVehicle(ID3D11Device* pDevice, const std::wstring& assetFile, std::vector<Texture*> pTextures)
		: BaseMaterial(pDevice, assetFile)
	{
		SetResourceVariable();
		
		SetTexture(m_pDiffuseMapVariable, pTextures[static_cast<int>(TextureType::diffuse)]);
		SetTexture(m_pNormalMapVariable, pTextures[static_cast<int>(TextureType::normal)]);
		SetTexture(m_pSpecularMapVariable, pTextures[static_cast<int>(TextureType::specular)]);
		SetTexture(m_pGlossinessMapVariable, pTextures[static_cast<int>(TextureType::gloss)]);
	}

	MaterialVehicle::~MaterialVehicle()
	{
	}

	void MaterialVehicle::SetResourceVariable()
	{
		//difuse map
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid\n";
		}

		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
		{
			std::wcout << L"m_pNormalMapVariable not valid\n";
		}

		//specular variable
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
		{
			std::wcout << L"m_pSpecularMapVariable not valid\n";
		}

		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessMapVariable not valid\n";
		}
	}

	void MaterialVehicle::SetTexture(ID3DX11EffectShaderResourceVariable* pResourceVariable, Texture* ptexture)
	{
		if (pResourceVariable != nullptr)
		{
			pResourceVariable->SetResource(ptexture->GetResourceView());
		}
	}
}
