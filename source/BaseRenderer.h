#pragma once
#include <cstdint>
#include <vector>
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class BaseRenderer

	{
	public:
		BaseRenderer(SDL_Window* pWindow);
		virtual ~BaseRenderer() {};

		BaseRenderer(const BaseRenderer&) = delete;
		BaseRenderer(BaseRenderer&&) noexcept = delete;
		BaseRenderer& operator=(const BaseRenderer&) = delete;
		BaseRenderer& operator=(BaseRenderer&&) noexcept = delete;

		//abstract functions must be overidden
		virtual void Update(const Timer* pTimer) = 0;
		virtual void Render() const = 0;

	protected:
		SDL_Window* m_pWindow{};
		int m_Width{};
		int m_Height{};
	};
}

