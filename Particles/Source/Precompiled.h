#pragma once

// Contains includes that should be put in the PCH file.
// Author: Andreas Larsson
// Date: 2016-12-12 (YYYY-MM-DD)

// NOTE: STD library
#include <limits>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <unordered_map>

// NOTE: Windows
#include <Windows.h>
#include <wrl/client.h>

// NOTE: Direct3D
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>

// NOTE: Asset import library
#include <assimp/LogStream.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// NOTE: DirectX utilities
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>