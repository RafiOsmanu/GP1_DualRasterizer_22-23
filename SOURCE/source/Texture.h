#pragma once
#include <SDL_surface.h>
#include <string>


namespace dae
{
	struct Vector2;

	class Texture
	{
	public:

		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);
		~Texture();
		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);
		void ResourceCreation(ID3D11Device* pDevice, SDL_Surface* pSurface);
		ID3D11ShaderResourceView* GetResourceView();
		ColorRGB Sample(const Vector2& uv) const;
		Vector3 SampleNormal(const Vector2& uv) const;

	private:

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
		ID3D11Texture2D* m_pResource;
		ID3D11ShaderResourceView* m_pSRV;
	};
}

