#include "pch.h"
#include "MeshHandler.h"

namespace dae
{
	MeshHandler::MeshHandler(ID3D11Device* pDevice, const Mesh& mesh, BaseMaterial* pMaterial)
		: m_pIndexBuffer{ nullptr }
		, m_pVertexBuffer{ nullptr }
		, m_numIndices{ 0 }
		, m_pMaterial{ pMaterial }
		, m_pInputLayout{ nullptr }
		, m_Vertices{ mesh.vertices }
		, m_Indices{ mesh.indices }
		
	{
		
		//Create Vertex Layout
		static constexpr uint32_t numElements{ 5 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "WORLD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 24;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "NORMAL";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 32;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[4].SemanticName = "TANGENT";
		vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[4].AlignedByteOffset = 44;
		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;



		//Create vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(m_Vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_Vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		//Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		m_pMaterial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

		result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			assert(false); //or return

		//Create index buffer
		m_numIndices = static_cast<uint32_t>(m_Indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = m_Indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

		if (FAILED(result))
			return;


	}

	MeshHandler::~MeshHandler()
	{
		delete  m_pMaterial;
		m_pVertexBuffer->Release();
		m_pIndexBuffer->Release();
		m_pInputLayout->Release();
	}


	void MeshHandler::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera, const Mesh& mesh)
	{
		//1. Set PrimitiveTopolgy
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//2. Set Input Layout
		pDeviceContext->IASetInputLayout(m_pInputLayout);

		//3. Set VertexBuffer
		constexpr UINT stride = sizeof(Vertex_PosCol);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		//4. Set IndexBuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//5. update worldViewProjectMatrix
		Matrix worldViewProjectionMatrix{ mesh.worldMatrix * pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix() };
		m_pMaterial->SetMatrix(worldViewProjectionMatrix, mesh.worldMatrix, pCamera->GetInvViewMatrix());

		//6. Draw
		D3DX11_TECHNIQUE_DESC techDesc{};
		//from my own Effect Class
		m_pMaterial->GetTechnique()->GetDesc(&techDesc);

		//from material class
		m_pMaterial->GetTechnique()->GetPassByIndex(pass)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_numIndices, 0, 0);
		

	}

	void MeshHandler::TogglePass()
	{
		pass = (pass + 1) % 3;
	}
}