#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "CombinedRenderer.h"

using namespace dae;
void PrintKeyBinds();

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - ***Rafi Osmanu/GD15***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new CombinedRenderer(pWindow);

	//print keybinds Info
	PrintKeyBinds();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
					pRenderer->SwitchProcces();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F2)
					pRenderer->ToggleVehicleRotation();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F3)
					pRenderer->ToggleCombustionRender();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F4)
					pRenderer->ToggleTechniquePass();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F5)
					pRenderer->ToggleSWRenderOutput();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F6)
					pRenderer->ToggleNormalMapSW();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F7)
					pRenderer->ToggleDepthVisualization();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F8)
					pRenderer->ToggleBoundingBoxesVisual();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F9)
					pRenderer->ToggleCullMode();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F10)
					pRenderer->ToggleUniform();
				else if (e.key.keysym.scancode == SDL_SCANCODE_F11)
					pTimer->ToggleFps();
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			pTimer->PrintFps();
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}


void PrintKeyBinds()
{
	//SHARED
	std::cout << "[Key Bindings - SHARED]" << '\n';
	std::cout << "    [F1] Toggle Rasterizer Mode(HARDWARE / SOFTWARE)" << '\n';
	std::cout << "    [F2]  Toggle Vehicle Rotation(ON / OFF)" << '\n';
	std::cout << "    [F9]  Cycle CullMode(BACK / FRONT / NONE) " << '\n';
	std::cout << "    [F10] Toggle Uniform ClearColor(ON / OFF)" << '\n';
	std::cout << "    [F11] Toggle Print FPS(ON / OFF)" << '\n';

	//HARDWARE
	std::cout << "[Key Bindings - HARDWARE]" << '\n';
	std::cout << "    [F3] Toggle FireFX(ON / OFF)" << '\n';
	std::cout << "    [F4] Cycle Sampler State(POINT / LINEAR / ANISOTROPIC)" << '\n';

	//SOFTWARE
	std::cout << "[Key Bindings - SOFTWARE]" << '\n';
	std::cout << "    [F5] Cycle Shading Mode(COMBINED / OBSERVED_AREA / DIFFUSE / SPECULAR)" << '\n';
	std::cout << "    [F6] Toggle NormalMap(ON / OFF)" << '\n';
	std::cout << "    [F7] Toggle DepthBuffer Visualization(ON / OFF)" << '\n';
	std::cout << "    [F8] Toggle BoundingBox Visualization(ON / OFF)" << '\n';
	std::cout << '\n';

}