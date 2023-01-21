#pragma once
#include "pch.h"

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{}; //W3
		Vector3 normal{}; //W4
		Vector3 tangent{}; //W4
		Vector3 viewDirection{}; //W4
	};

	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		std::vector<Vertex_PosCol> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};
		void Translate(const Vector3& translation)
		{
			worldMatrix *= Matrix::CreateTranslation(translation);
		}

		void RotateMesh(const Timer* timer)
		{
			const auto yawAngle = (PI_DIV_4 * (timer->GetTotal()));

			const Matrix rotMatrix{ Matrix::CreateRotationY(yawAngle) };

			worldMatrix = rotMatrix;
		}
	};

	enum class TextureType
	{
		diffuse = 0, normal = 1, specular = 2, gloss = 3
	};
}

