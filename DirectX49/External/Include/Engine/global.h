#pragma once

// ǥ�� ���̺귯��
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

// DirectX 11 ��� �� ���̺귯�� ����
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;

#pragma comment(lib,"d3d11")
#pragma comment(lib,"d3dcompiler")

// ���� ��� ����
#include "singleton.h"
#include "define.h"
#include "struct.h"
