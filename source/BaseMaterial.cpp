#include "pch.h"
#include "BaseMaterial.h"
#include <assert.h>

namespace dae 
{
	BaseMaterial::BaseMaterial(ID3D11Device* pDevice, const std::wstring& assetFile)
		:m_pEffect{ LoadEffect(pDevice, assetFile) }
	{
		//m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
		m_pTechnique = m_pEffect->GetTechniqueByName("Techniques");
		if (!m_pTechnique->IsValid())
		{
			std::wcout << L"Technique not valid\n";
		}

		//worldviewprojectionMatrix
		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid())
		{
			std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
		}

		//worldMatrix
		m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pMatWorldVariable->IsValid())
		{
			std::wcout << L"m_pMatWorldVariable not valid\n";
		}

		//onb of camera/ inverse view
		m_pMatInvViewVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
		if (!m_pMatInvViewVariable->IsValid())
		{
			std::wcout << L"m_pMatInvViewVariable not valid\n";
		}

	}

	BaseMaterial::~BaseMaterial()
	{
		m_pEffect->Release();
	}


	ID3DX11EffectTechnique* BaseMaterial::GetTechnique()
	{
		return m_pTechnique;
	}

	void BaseMaterial::SetMatrix(const Matrix& worldViewProjectMatrix, const Matrix& worldMatrix, const Matrix& invViewMatrix)
	{
		m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectMatrix));
		m_pMatWorldVariable->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
		m_pMatInvViewVariable->SetMatrix(reinterpret_cast<const float*>(&invViewMatrix));
	}


	ID3DX11Effect* BaseMaterial::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile(assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
					ss << pErrors[i];

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;

			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CReateEffectFile!\npath: " << assetFile;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}
		return pEffect;
	}
}

