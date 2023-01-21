#pragma once
#include "Utils.h"
#include "Math.h"
#include "BaseMaterial.h"

namespace dae
{
	class MeshHandler
	{
	public:
		MeshHandler(ID3D11Device* pDevice, const Mesh& mesh, BaseMaterial* pMaterial);
		~MeshHandler();
		void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera, const Mesh& mesh);
		void TogglePass();

	private:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11InputLayout* m_pInputLayout;
		uint32_t m_numIndices;
		std::vector<Vertex_PosCol> m_Vertices{};
		std::vector<uint32_t> m_Indices{};
		BaseMaterial* m_pMaterial{};
		std::unique_ptr<Texture> m_pDiffuseCombustionTexture{ nullptr };
		UINT pass{ 0 };
		Matrix m_WorldMatrix;
	};
}

