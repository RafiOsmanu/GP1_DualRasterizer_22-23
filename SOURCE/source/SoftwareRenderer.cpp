//External includes
#include "pch.h"
#include "SDL.h"
#include "SDL_surface.h"
#include <ppl.h>

//Project includes
#include "SoftwareRenderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Texture.h"
#include "Utils.h"


using namespace dae;

SoftwareRenderer::SoftwareRenderer(SDL_Window* pWindow)
	: BaseRenderer(pWindow)
	, m_pMesh{nullptr}
	, m_IsUniform{false}
	, m_CullMode{CullMode::backFace}
{
	
	//init depthBuffer
	m_pDepthBufferPixels = new float[m_Width * m_Height]();

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	m_AspectRatio = float(m_Width) / float(m_Height);

	m_IsMeshLoadedIn = false;
	m_CurrentRenderState = RenderState::combined;
	m_BoundingBoxVisualization = false;
};

SoftwareRenderer::~SoftwareRenderer()
{
	if (m_pFrontBuffer)
	{
		SDL_FreeSurface(m_pFrontBuffer);
		m_pFrontBuffer = nullptr;
	}

	if (m_pBackBuffer)
	{
		SDL_FreeSurface(m_pBackBuffer);
		m_pBackBuffer = nullptr;
	}
	delete[] m_pDepthBufferPixels;
}

void SoftwareRenderer::Update(Timer* pTimer)
{
	m_pCamera->Update(pTimer);
}

void SoftwareRenderer::Render()
{
	//init Depth Buffer with FLT_MAX
	std::fill_n(m_pDepthBufferPixels, (m_Width * m_Height), FLT_MAX);
	
	//clear buffer
	if(!m_IsUniform)
	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 255 * COLOR_SOFTWARE.r, 255 * COLOR_SOFTWARE.g, 255 * COLOR_SOFTWARE.b));
	else
	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 255 * COLOR_UNIFORM.r, 255 * COLOR_UNIFORM.g, 255 * COLOR_UNIFORM.b));


	//@START
	
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	render_W4_Part1();  //PixelShading

	//@END
	
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void SoftwareRenderer::VertexTransformationFunction(Mesh& mesh) const
{
	//Todo > W1 Projection Stage
	Matrix worldViewProjectionMatrix;
	Matrix WorldMatrix{ mesh.worldMatrix };
	//vertices_out = vertices_in;
	for (auto& vertexPosCol : mesh.vertices)
	{
		worldViewProjectionMatrix = WorldMatrix * m_pCamera->GetViewProjectionMatrix();

		//Vector4 newVertex{vertexOut.position.x, vertexOut.position.y, vertexOut.position.z, 1 };
		Vertex_Out newVertexOut
		{
			worldViewProjectionMatrix.TransformPoint({vertexPosCol.position, 1.f}),
			vertexPosCol.color,
			vertexPosCol.uv,
			mesh.worldMatrix.TransformVector(vertexPosCol.normal).Normalized(),
			mesh.worldMatrix.TransformVector(vertexPosCol.tangent).Normalized(),
			(mesh.worldMatrix.TransformVector(vertexPosCol.position) - m_pCamera->GetOrigin()).Normalized()
		};


		//perspective divide
		newVertexOut.position.x /= newVertexOut.position.w;
		newVertexOut.position.y /= newVertexOut.position.w;
		newVertexOut.position.z /= newVertexOut.position.w;
		newVertexOut.position.w = newVertexOut.position.w;

		mesh.vertices_out.emplace_back(newVertexOut);

	}
	
}

bool dae::SoftwareRenderer::IsInTriangle(const std::vector<Vector2>& verticesScreenspace, const Vector2& pixelPos)
{

	Vector2 v0{ verticesScreenspace[0].x, verticesScreenspace[0].y };
	Vector2 v1{ verticesScreenspace[1].x, verticesScreenspace[1].y };
	Vector2 v2{ verticesScreenspace[2].x, verticesScreenspace[2].y };
	

	Vector2 edgeA{ v1 - v0 };
	Vector2 edgeB{ v2 - v1 };
	Vector2 edgeC{ v0 - v2 };

	Vector2 pointToSideA{ pixelPos - v0 };
	Vector2 pointToSideB{ pixelPos - v1 };
	Vector2 pointToSideC{ pixelPos - v2 };


	if (Vector2::Cross(edgeA, pointToSideA) < 0) return false;
	if (Vector2::Cross(edgeB, pointToSideB) < 0) return false;
	if (Vector2::Cross(edgeC, pointToSideC) < 0) return false;

	return true;
}

void SoftwareRenderer::render_W4_Part1()
{
	//clear global mesh variable
	m_pMesh->vertices_out.clear();

	VertexTransformationFunction(*m_pMesh);

	

	//int count{};
	const uint32_t amountOfTriangles = ((uint32_t)m_pMesh->indices.size())/3;
	Concurrency::parallel_for(static_cast<uint32_t>(0), amountOfTriangles,[=, this](int i)
	{

			uint32_t indc0{ m_pMesh->indices[3 * i] };
			uint32_t indc1{ m_pMesh->indices[3 * i + 1]};
			uint32_t indc2{ m_pMesh->indices[3 * i + 2]};


			//cach vertices
			//vertex_out
			Vertex_Out v0 = m_pMesh->vertices_out[indc0];;
			Vertex_Out v1 = m_pMesh->vertices_out[indc1];;
			Vertex_Out v2 = m_pMesh->vertices_out[indc2];;

			

			//fustrum culling
			if (FustrumCulling(v0.position, v1.position, v2.position)) return;

			//screenSpace
			ToScreenSpace(v0.position, v1.position, v2.position);
			
			//Vector2 for cross
			const Vector2 vec0{ v0.position.x, v0.position.y };
			const Vector2 vec1{ v1.position.x, v1.position.y };
			const Vector2 vec2{ v2.position.x, v2.position.y };

			//vertices 2D
			std::vector<Vector2> vertices{};
			vertices.resize(3);
			vertices[0] = vec0;
			vertices[1] = vec1;
			vertices[2] = vec2;

			//boundingBox Optimization
			Vector2 topLeft{};
			Vector2 bottomRight{};

			BoundingBox(topLeft, bottomRight, vertices);

			//RENDER LOGIC
			for (int px{ static_cast<int>(topLeft.x) }; px <= static_cast<int>(bottomRight.x); ++px)
			{
				for (int py{ static_cast<int>(topLeft.y) }; py <= static_cast<int>(bottomRight.y); ++py)
				{
					ColorRGB finalColor{};

					if (m_BoundingBoxVisualization)
					{
						finalColor = colors::White;

						//Update Color in Buffer
						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));
					}
					else
					{
						Vector2 currentPixel = { static_cast<float>(px) + 0.5f , static_cast<float>(py) + 0.5f };

						float weight0 = 0, weight1 = 0, weight2 = 0;
						float triangleSide0 = 0, triangleSide1 = 0, triangleSide2 = 0;

						float area{ Vector2::Cross((vec1 - vec0), (vec2 - vec0)) };

						//calculate weights
						//isInTriangleCheck

						Vector2 edge0{ vec2 - vec1 };
						Vector2 edge1{ vec0 - vec2 };
						Vector2 edge2{ vec1 - vec0 };

						weight0 = triangleSide0 = Vector2::Cross(edge0, (currentPixel - vec1));
						weight1 = triangleSide1 = Vector2::Cross(edge1, (currentPixel - vec2));
						weight2 = triangleSide2 = Vector2::Cross(edge2, (currentPixel - vec0));


						weight0 /= area;
						weight1 /= area;
						weight2 /= area;
						float totalTriangleWeight{ weight0 + weight1 + weight2 };
						float invTotalTriangleWeight{ 1 / totalTriangleWeight };


						weight0 *= invTotalTriangleWeight;
						weight1 *= invTotalTriangleWeight;
						weight2 *= invTotalTriangleWeight;

						bool isBackFace{ triangleSide0 < 0.f && triangleSide1 < 0.f && triangleSide2 < 0.f };
						bool isFrontFace{ triangleSide0 >= 0.f && triangleSide1 >= 0.f && triangleSide2 >= 0.f };

						//CullMode check
						switch (m_CullMode)
						{
						case SoftwareRenderer::CullMode::backFace:
							if (isBackFace) continue;
							break;
						case SoftwareRenderer::CullMode::frontFace:
							if (isFrontFace) continue;
							break;
						case SoftwareRenderer::CullMode::none:
							break;
						}

						if (!isBackFace && !isFrontFace) continue;

						//depth
						//interpolatedZ
						const float interpolatedZ =
							1 / ((1 / v0.position.z) * weight0 +
								(1 / v1.position.z) * weight1 +
								(1 / v2.position.z) * weight2);


						if (interpolatedZ < m_pDepthBufferPixels[(py * m_Width) + px])
						{
							//interpolatedW
							const float interpolatedW =
								1 / ((1 / v0.position.w) * weight0 +
									(1 / v1.position.w) * weight1 +
									(1 / v2.position.w) * weight2);



							//Position
							const Vector4 pos = { ((v0.position * weight0) + (v1.position * weight1) + (v2.position * weight2)) * interpolatedW };


							//uv
							const Vector2 interpolatedUv =
								((v0.uv / v0.position.w) * weight0
									+ (v1.uv / v1.position.w) * weight1
									+ (v2.uv / v2.position.w) * weight2) * interpolatedW;

							//normal
							const Vector3 normal{ ((v0.normal / (v0.position.w)) * weight0
								+ (v1.normal / v1.position.w) * weight1
								+ (v2.normal / v2.position.w) * weight2) * interpolatedW };

							//tangent
							const Vector3 tangent = { ((v0.tangent / v0.position.w) * weight0
									+ (v1.tangent / v1.position.w) * weight1
									+ (v2.tangent / v2.position.w) * weight2) * interpolatedW };

							// viewDirection
							const Vector3 viewDirection = { (((v0.viewDirection / v0.position.w) * weight0
									+ (v1.viewDirection / v1.position.w) * weight1
									+ (v2.viewDirection / v2.position.w) * weight2) * interpolatedW) };




							m_pDepthBufferPixels[(py * m_Width) + px] = interpolatedZ;

							//pixel shading
							Vertex_Out finalPixel{ pos, finalColor, interpolatedUv, normal, tangent, viewDirection };

							//sample pixel color
							ColorRGB sampledColor = m_pDiffuseTexture->Sample(interpolatedUv);

							switch (m_DepthBuffer)
							{
							case false:
								finalColor = PixelShading(finalPixel);
								break;
							case true:
								float remapedValue{ Remap(interpolatedZ, 0.985f, 1.f) };
								finalColor = { remapedValue, remapedValue, remapedValue };
								break;
							}

							//Update Color in Buffer
							finalColor.MaxToOne();

							m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(finalColor.r * 255),
								static_cast<uint8_t>(finalColor.g * 255),
								static_cast<uint8_t>(finalColor.b * 255));
						}
					}
				}
			}
			

	});
}

float SoftwareRenderer::Remap(float value, float minValue, float maxValue) 
{
	//map to range[0,1]
	return (value - minValue) / (maxValue - minValue);
}

bool SoftwareRenderer::FustrumCulling(const Vector3 v0, const Vector3 v1, const Vector3 v2)
{
	if ((v0.x < -1 || v0.x > 1) ||
		(v0.y < -1 || v0.y > 1) ||
		(v0.z < 0 || v0.z > 1) ||
		(v1.x < -1 || v1.x > 1) ||
		(v1.y < -1 || v1.y > 1) ||
		(v1.z < 0 || v1.z > 1) ||
		(v2.x < -1 || v2.x > 1) ||
		(v2.y < -1 || v2.y > 1) ||
		(v2.z < 0 || v2.z > 1))
	{
		return true;
	}
	return false;
}

void SoftwareRenderer::ToScreenSpace(Vector4& v0, Vector4& v1, Vector4& v2)
{
	//v0
	v0.x = (v0.x + 1) / 2 * float(m_Width);
	v0.y = (1 - v0.y) / 2 * float(m_Height);

	//v1
	v1.x = (v1.x + 1) / 2 * float(m_Width);
	v1.y = (1 - v1.y) / 2 * float(m_Height);

	//v2
	v2.x = (v2.x + 1) / 2 * float(m_Width);
	v2.y = (1- v2.y) / 2 * float(m_Height);
}

bool SoftwareRenderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

void SoftwareRenderer::BoundingBox(Vector2& topLeft, Vector2& bottomRight, std::vector<Vector2> v)
{
	//bounding box optimization

	//top left
	//x
	topLeft.x = std::min(v[0].x, std::min(v[1].x, v[2].x));
	if (topLeft.x <= 0) topLeft.x = 0;
	if (topLeft.x >= m_Width) topLeft.x = static_cast<float>(m_Width) - 1;

	//y
	topLeft.y = std::min(v[0].y, std::min(v[1].y, v[2].y));
	if (topLeft.y <= 0) topLeft.y = 0;
	if (topLeft.y >= m_Height) topLeft.y = static_cast<float>(m_Height) - 1;

	//bottomRight
	
	//x
	bottomRight.x = std::max(v[0].x, std::max(v[1].x, v[2].x));
	if (bottomRight.x <= 0) bottomRight.x = 0;
	if (bottomRight.x >= (m_Width - 1)) bottomRight.x = static_cast<float>(m_Width) - 1;

	//y
	bottomRight.y = std::max(v[0].y, std::max(v[1].y, v[2].y));
	if (bottomRight.y <= 0) bottomRight.y = 0;
	if (bottomRight.y >= (m_Height - 1)) bottomRight.y = static_cast<float>(m_Height) - 1;
}



ColorRGB SoftwareRenderer::PixelShading(const Vertex_Out& v)
{
	Vector3 lightDirection = { .577f, -.577f, .577f };
	const float lightIntensity{ 7.f };
	ColorRGB ambient{ 0.025f, 0.025f , 0.025f };
	//diffuse reflectivity
	const float kd{1.f};

	Vector3 sampledNormal{v.normal};
	
	Vector3 binormal = Vector3::Cross(v.normal, v.tangent);
	Matrix tangentSpaceAxis = Matrix{ v.tangent, binormal.Normalized(), v.normal, Vector3::Zero};
	sampledNormal = m_pNormalTexture->SampleNormal(v.uv);
	sampledNormal = 2 * sampledNormal - Vector3(1.f, 1.f, 1.f);
	sampledNormal = tangentSpaceAxis.TransformVector(sampledNormal);
	sampledNormal.Normalize();
	
	

	//observedArea
	float observedArea{};
	if (m_NormalMapToggle) observedArea = Vector3::Dot(sampledNormal, -lightDirection);

	else observedArea = Vector3::Dot(v.normal, -lightDirection);

	if (observedArea < 0) observedArea = 0;

	//Diffuse
	const ColorRGB lambertDiffuse{ (kd * m_pDiffuseTexture->Sample(v.uv)) / float(M_PI) };

	//phong 
	const float shininess{ 25.f };
	const ColorRGB specularColor{ m_pSpecularTexture->Sample(v.uv) };
	const float phongExp{ m_pGlossTexture->SampleNormal(v.uv).x * shininess };

	const Vector3 reflect{ Vector3::Reflect(-lightDirection, sampledNormal) };
	float cosAngle{ Vector3::Dot(reflect, v.viewDirection) };
	if (cosAngle < 0.f) cosAngle = 0.f;

	const float specReflection{ kd * powf(cosAngle, phongExp) };
	const ColorRGB phong{ specReflection * specularColor };


	switch (m_CurrentRenderState)
	{
	case dae::SoftwareRenderer::RenderState::observedArea:
		
		return ColorRGB(observedArea, observedArea, observedArea);

	case dae::SoftwareRenderer::RenderState::lambert:

		return lightIntensity * lambertDiffuse * observedArea;

	case dae::SoftwareRenderer::RenderState::phong:

		return phong * observedArea;

	case dae::SoftwareRenderer::RenderState::combined:

		return ColorRGB(lightIntensity * lambertDiffuse * observedArea + ambient + phong);
	}
	return ColorRGB();
}

//with int
void SoftwareRenderer::ToggleDepthBuffer()
{
	if (m_DepthBuffer)
	{
		m_DepthBuffer = false;
		std::cout << "**(SOFTWARE) DepthBuffer Visualization OFF\n";
	}
	else
	{
		m_DepthBuffer = true;
		std::cout << "**(SOFTWARE) DepthBuffer Visualization ON\n";
	}
}

//with enum
void SoftwareRenderer::ToggleRenderOutput()
{
	m_CurrentRenderState = RenderState((int(m_CurrentRenderState) + 1) % 4);

	switch (m_CurrentRenderState)
	{
	case dae::SoftwareRenderer::RenderState::observedArea:
		std::cout << "**(SOFTWARE) Shading Mode = OBSERVED_AREA\n";
		break;

	case dae::SoftwareRenderer::RenderState::lambert:
		std::cout << "**(SOFTWARE) Shading Mode = DIFFUSE\n";
		break;

	case dae::SoftwareRenderer::RenderState::phong:
		std::cout << "**(SOFTWARE) Shading Mode = SPECULAR\n";
		break;

	case dae::SoftwareRenderer::RenderState::combined:
		std::cout << "**(SOFTWARE) Shading Mode = COMBINED\n";
		break;
	}
}

//with bool
void dae::SoftwareRenderer::ToggleNormalMap()
{
	if (!m_NormalMapToggle)
	{
		m_NormalMapToggle = true;
		std::cout << "**(SOFTWARE) NormalMap ON\n";
	}
	else
	{
		m_NormalMapToggle = false;
		std::cout << "**(SOFTWARE) NormalMap OFF\n";
	}
}

void dae::SoftwareRenderer::ToggleBoundingBox()
{
	if (m_BoundingBoxVisualization)
	{
		m_BoundingBoxVisualization = false;
		std::cout << "**(SOFTWARE) BoundingBox Visualization OFF\n";
	}
	else 
	{ 
		m_BoundingBoxVisualization = true;
		std::cout << "**(SOFTWARE) BoundingBox Visualization ON\n";
	}
}

void dae::SoftwareRenderer::ToggleUniformColor()
{
	if (m_IsUniform)
	{
		m_IsUniform = false;
		std::cout << "**(SHARED) Uniform ClearColor OFF\n";
	}

	else
	{
		m_IsUniform = true;
		std::cout << "**(SHARED) Uniform ClearColor ON\n";
	}
}

void dae::SoftwareRenderer::SWToggleCullMode()
{
	m_CullMode = CullMode((int(m_CullMode) + 1) % int(CullMode::size));

	switch (m_CullMode)
	{
	case SoftwareRenderer::CullMode::backFace:
		std::cout << "**(SHARED) CullMode = BACK\n";
		break;
	case SoftwareRenderer::CullMode::frontFace:
		std::cout << "**(SHARED) CullMode = FRONT\n";
		break;
	case SoftwareRenderer::CullMode::none:
		std::cout << "**(SHARED) CullMode = NONE\n";
		break;
	}


}

void dae::SoftwareRenderer::Initialize(std::vector<Texture*>& mainTextures, Camera* pCamera, Mesh& vehicleMesh)
{
	m_pCamera = pCamera;
	m_pMesh = &vehicleMesh;

	m_pDiffuseTexture = mainTextures[static_cast<int>(TextureType::diffuse)];
	m_pNormalTexture = mainTextures[static_cast<int>(TextureType::normal)];
	m_pSpecularTexture = mainTextures[static_cast<int>(TextureType::specular)];
	m_pGlossTexture = mainTextures[static_cast<int>(TextureType::gloss)];
}




	
		