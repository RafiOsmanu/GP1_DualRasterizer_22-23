#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <memory>
#define NOMINMAX  //for directx

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Framework Headers
#include "Timer.h"
#include "Math.h"
#include "ColorRGB.h"

//colors
#define COLOR_UNIFORM ColorRGB{.1f, .1f, .1f}
#define COLOR_SOFTWARE ColorRGB{.39f, .39f, .39f}
#define COLOR_HARDWARE ColorRGB{.39f, .59f, .93f}