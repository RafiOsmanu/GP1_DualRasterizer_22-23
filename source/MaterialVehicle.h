#pragma once
#include "Camera.h"
#include"Texture.h"
#include "BaseMaterial.h"

namespace dae {

	class MaterialVehicle : public BaseMaterial
	{
	public:
		MaterialVehicle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::wstring& assetFile, std::vector<Texture*> pTextures);
		virtual ~MaterialVehicle();
		void ToggleCullMode();

	private:
		void SetResourceVariable();
		void SetTexture(ID3DX11EffectShaderResourceVariable* pResourceVariable, Texture* ptexture);
		
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		

		ID3DX11EffectRasterizerVariable* m_pRasterizerStateVariable;

		enum class CullMode
		{
			frontFace, backFace, none
		};

		CullMode m_CullMode{CullMode::backFace};

		void SetCullMode();

		
	};
}


