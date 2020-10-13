#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <memory>
#include <vector>
#include <io.h>

#include <windows.h>
#include <assert.h>

#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "vector3.h"

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "xaudio2.lib")


#define SCREEN_WIDTH	(960)			// ウインドウの幅
#define SCREEN_HEIGHT	(540)			// ウインドウの高さ


HWND GetWindow();
