#pragma once

// 표준 라이브러리
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;

#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#include<wrl.h>
using namespace Microsoft::WRL;

// DirectX 11 헤더 및 라이브러리 참조
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;

#pragma comment(lib,"d3d11")
#pragma comment(lib,"d3dcompiler")

// 엔진 헤더 참조
#include "singleton.h"
#include "define.h"
#include "struct.h"
