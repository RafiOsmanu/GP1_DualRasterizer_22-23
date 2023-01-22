#include "pch.h"
#include "MaterialVehicle.h"
#include "Texture.h"
#include "DataTypes.h"


namespace dae
{
	MaterialVehicle::MaterialVehicle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::wstring& assetFile, std::vector<Texture*> pTextures)
		: BaseMaterial(pDevice, assetFile)
		,m_pDevice{pDevice}
		,m_pDeviceContext{pDeviceContext}
	{
		SetResourceVariable();
		
		SetTexture(m_pDiffuseMapVariable, pTextures[static_cast<int>(TextureType::diffuse)]);
		SetTexture(m_pNormalMapVariable, pTextures[static_cast<int>(TextureType::normal)]);
		SetTexture(m_pSpecularMapVariable, pTextures[static_cast<int>(TextureType::specular)]);
		SetTexture(m_pGlossinessMapVariable, pTextures[static_cast<int>(TextureType::gloss)]);
	}

	MaterialVehicle::~MaterialVehicle()
	{
		m_pDiffuseMapVariable->Release();
		m_pNormalMapVariable->Release();
		m_pSpecularMapVariable->Release();
		m_pGlossinessMapVariable->Release();
		m_pRasterizerStateVariable->Release();
	}

	void MaterialVehicle::ToggleCullMode()
	{
		m_CullMode = CullMode((int(m_CullMode) + 1) % 3);
		SetCullMode();
	}

	void MaterialVehicle::SetResourceVariable()
	{
		//difuse map
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid\n";
		}

		//normalMap Variable
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

		//gloss variable
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessMapVariable not valid\n";
		}

		//CullMode Variable
		m_pRasterizerStateVariable = m_pEffect->GetVariableByName("gRasterizerState")->AsRasterizer();
		if (!m_pRasterizerStateVariable->IsValid())
		{
			std::wcout << L"m_pRasterizerStat not valid\n";
		}


	}

	void MaterialVehicle::SetTexture(ID3DX11EffectShaderResourceVariable* pResourceVariable, Texture* ptexture)
	{
		if (pResourceVariable != nullptr)
		{
			pResourceVariable->SetResource(ptexture->GetResourceView());
		}
	}

	void MaterialVehicle::SetCullMode()
	{
		ID3D11RasterizerState* rasterizerState{nullptr};
		D3D11_RASTERIZER_DESC rasterizerDesc{};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;

		switch (m_CullMode)
		{
		case CullMode::backFace:
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			std::cout << "BackFace Culling" << "\n";
			break;

		case CullMode::frontFace:
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			std::cout << "FrontFace Culling" << "\n";
			break;

		case CullMode::none:
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			std::cout << "No Culling" << "\n";
			break;
		}


		HRESULT hr = 
		m_pDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

		if (FAILED(hr))
		{
			std::cout << "OOPS! Looks Like You Failed Kid" << "\n";
		}

		m_pDeviceContext->RSSetState(rasterizerState);

		if(rasterizerState)
		m_pRasterizerStateVariable->SetRasterizerState(0, rasterizerState);

		rasterizerState->Release();
	}
}
