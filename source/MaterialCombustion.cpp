#include "pch.h"
#include "MaterialCombustion.h"

namespace dae
{
	MaterialCombustion::MaterialCombustion(ID3D11Device* pDevice, const std::wstring& assetFile, Texture* ptexture)
		:BaseMaterial(pDevice, assetFile)
	{
		SetResourceVariable();
		SetTexture(pDevice, m_pDiffuseMapVariable, ptexture);
	}

	MaterialCombustion::~MaterialCombustion()
	{
	}

	void MaterialCombustion::SetResourceVariable()
	{
		//difuse map
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid\n";
		}
	}

	void MaterialCombustion::SetTexture(ID3D11Device* pDevice, ID3DX11EffectShaderResourceVariable* pResourceVariable, Texture* ptexture)
	{
		if (pResourceVariable)
		{
			pResourceVariable->SetResource(ptexture->GetResourceView());
		}
	}
}
