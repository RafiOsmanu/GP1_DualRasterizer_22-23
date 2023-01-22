#pragma once
#include "Camera.h"
#include "Texture.h"

namespace dae {

	class BaseMaterial
	{
	public:
		BaseMaterial(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~BaseMaterial();
		ID3DX11EffectTechnique* GetTechnique();
		void SetMatrix(const Matrix& worldViewProjectMatrix, const Matrix& worldMatrix, const Matrix& invViewMatrix);
		
	protected:
		ID3DX11Effect* m_pEffect;
	private:

		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		ID3DX11EffectTechnique* m_pTechnique;
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
		ID3DX11EffectMatrixVariable* m_pMatWorldVariable;
		ID3DX11EffectMatrixVariable* m_pMatInvViewVariable;
	};
}

