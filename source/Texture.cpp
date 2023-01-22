#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <iostream>
#include <assert.h>

namespace dae
{
	Texture::Texture(ID3D11Device* pDevice, SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels },
		m_pResource{},
		m_pSRV{nullptr}
	{
		ResourceCreation(pDevice, pSurface);
	}

	Texture::~Texture()
	{	
		m_pResource->Release();
		m_pSRV->Release();
		
		SDL_FreeSurface(m_pSurface);
	}

	Texture* Texture::LoadFromFile(ID3D11Device* pDevice, const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		SDL_Surface* data = IMG_Load(path.c_str());

		//Create & Return a new Texture Object (using SDL_Surface)
		if (data == nullptr)
		{
			std::cout << "surface is nullptr" << "\n";
		}

		return new Texture(pDevice, data);

	}

	void Texture::ResourceCreation(ID3D11Device* pDevice, SDL_Surface* pSurface)
	{		
		//Resource
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);

		// resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pResource,&SRVDesc, &m_pSRV);
	}

	ID3D11ShaderResourceView* Texture::GetResourceView()
	{
		return m_pSRV;
	}

	//for software

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv
		Uint8 r, g, b;

		const Uint32 U{ static_cast<Uint32>(uv.x * m_pSurface->w) };
		const Uint32 V{ static_cast<Uint32>(uv.y * m_pSurface->h) };


		const Uint32 pixel{ m_pSurfacePixels[static_cast<Uint32>(U + (V * m_pSurface->w))] };

		SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);

		const float colorRemap{ 1 / 255.f };


		return { r * colorRemap, g * colorRemap, b * colorRemap };
	}

	Vector3 Texture::SampleNormal(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv
		Uint8 r, g, b;

		const Uint32 U{ static_cast<Uint32>(uv.x * m_pSurface->w) };
		const Uint32 V{ static_cast<Uint32>(uv.y * m_pSurface->h) };


		const Uint32 pixel{ m_pSurfacePixels[static_cast<Uint32>(U + (V * m_pSurface->w))] };

		SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);

		const float colorRemap{ 1 / 255.f };


		return { r * colorRemap, g * colorRemap, b * colorRemap };
	}

}
