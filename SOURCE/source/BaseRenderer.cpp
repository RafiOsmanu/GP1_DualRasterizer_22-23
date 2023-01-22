#include "pch.h"
#include "BaseRenderer.h"

dae::BaseRenderer::BaseRenderer(SDL_Window* pWindow)
	:m_pWindow{pWindow}
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
}
