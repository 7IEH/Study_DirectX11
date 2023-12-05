#include "pch.h"
#include "EHEngine.h"

#include "EHDevice.h"

#include "EHPathMgr.h"
#include "EHTimeMgr.h"
#include "EHKeyMgr.h"

Vtx triagnle[3] = {};

ComPtr<ID3D11Buffer> buffer = nullptr;

// vertexshader을 만들기 위한 이진 정보
ComPtr<ID3DBlob> g_VSBlob = nullptr;
ComPtr<ID3DBlob> g_PSBlob = nullptr;
ComPtr<ID3DBlob> g_ERRORBlob = nullptr;

ComPtr<ID3D11VertexShader> vertexShader = nullptr;
ComPtr<ID3D11PixelShader> pixelShader = nullptr;

ComPtr<ID3D11InputLayout> g_Layout = nullptr;

Engine::Engine()
	:m_hmainhWnd(nullptr)
	, m_vResolution({ 0,0 })
{

}

Engine::~Engine()
{

}

int Engine::Init(HWND _hWnd, POINT _vResolution)
{
	m_hmainhWnd = _hWnd;
	m_vResolution = _vResolution;


	RECT rt = { 0,0,m_vResolution.x,m_vResolution.y };

	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hmainhWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
	if (FAILED(Device::GetInst()->Init(m_hmainhWnd, m_vResolution)))
	{
		return E_FAIL;
	}

	// Manger init
	TimeMgr::GetInst()->Init();
	KeyMgr::GetInst()->Init();
	PathMgr::GetInst()->Init();


	// 삼각형 그리기--------------
	triagnle[0].vPos = Vec3(0.f, 1.f, 0.f);
	triagnle[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	triagnle[0].vUV = Vec2(0.f, 0.f);

	triagnle[1].vPos = Vec3(1.f, -1.f, 0.f);
	triagnle[1].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	triagnle[1].vUV = Vec2(0.f, 0.f);

	triagnle[2].vPos = Vec3(-1.f, -1.f, 0.f);
	triagnle[2].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	triagnle[2].vUV = Vec2(0.f, 0.f);
	//----------------------------

	// buffer에 옮기기 과정
	D3D11_BUFFER_DESC BufferDesc = {};
	// buffer 를 통해 보낼 정점의 정보 크기
	BufferDesc.ByteWidth = sizeof(Vtx) * 3;
	// 정점의 크기 정보를 나타냄
	BufferDesc.StructureByteStride = sizeof(Vtx);
	// 정점의 용도를 나타냄
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// GPU에 있는 정보를 수정 할 수 있게 끔
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = triagnle;

	DEVICE->CreateBuffer(&BufferDesc, &tSubData, buffer.GetAddressOf());

	// 정점 구조정보(Layout) 생성
	D3D11_INPUT_ELEMENT_DESC arrElement[3] = {};

	arrElement[0].InputSlot = 0;
	arrElement[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[0].SemanticName = "POSITION";
	arrElement[0].SemanticIndex = 0;
	arrElement[0].AlignedByteOffset = 0;
	arrElement[0].InstanceDataStepRate = 0;
	arrElement[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

	arrElement[1].InputSlot = 0;
	arrElement[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[1].SemanticName = "COLOR";
	arrElement[1].SemanticIndex = 0;
	arrElement[1].AlignedByteOffset = 12;
	arrElement[1].InstanceDataStepRate = 0;
	arrElement[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	arrElement[2].InputSlot = 0;
	arrElement[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrElement[2].SemanticName = "TEXCOORD";
	arrElement[2].SemanticIndex = 0;
	arrElement[2].AlignedByteOffset = 28;
	arrElement[2].InstanceDataStepRate = 0;
	arrElement[2].Format = DXGI_FORMAT_R32G32_FLOAT;

	

	// 버텍스 쉐이더
	// HLSL 버텍스 쉐이더 함수 컴파일
	// parameter
	// 1. 쉐이더 파일 경로
	// 2. nullptr
	// 3. D3D_COMPILE_STANDARD_INCLUDE
	// 4. entryPoint
	// 5. hlsl 파일 버전
	// 6. debug msg(D3DCOMPILE_DEBUG)
	// 7. 0
	// 8. 생성된 Blob 객체 반환
	// 9. error 용도 blob 객체 반환

	// Blob 파일 생성
	wstring path = PathMgr::GetInst()->GetPath();
	path += L"shader\\std2d.fx";
	D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,"VS_std2D","vs_5_0",D3DCOMPILE_DEBUG,0,
		g_VSBlob.GetAddressOf(),g_ERRORBlob.GetAddressOf());
	
	if (g_ERRORBlob != NULL)
	{
		cout << "VS_blob Error";
		return E_FAIL;
	}

	// VertexShader Create
	// bufferpointer , size, nullptr, 버텍스 정보
	DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

	// PixelBlob
	path = PathMgr::GetInst()->GetPath();
	path += L"shader\\std2d.fx";
	D3DCompileFromFile(path.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_std2D", "ps_5_0", D3DCOMPILE_DEBUG, 0,
		g_PSBlob.GetAddressOf(), g_ERRORBlob.GetAddressOf());

	if (g_ERRORBlob != NULL)
	{
		cout << "PS_blob Error";
		return E_FAIL;
	}

	DEVICE->CreatePixelShader(g_PSBlob->GetBufferPointer(), g_PSBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());

	// LayOut 생성
	DEVICE->CreateInputLayout(arrElement, 3, g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), g_Layout.GetAddressOf());

	return S_OK;
}

void Engine::Progress()
{
	float BackGroundColor[4] = { 1.f,1.f,1.f,1.f };
	Device::GetInst()->ClearRenderTarget(BackGroundColor);

	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;
	// IA 과정 -> 즉, GPU에 vertextbuffer를 가져다주는 작업이라 생각하면됨
	DEVICECONTEXT->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &iStride, &iOffset);
	DEVICECONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DEVICECONTEXT->IASetInputLayout(g_Layout.Get());

	// 만들어진 Shader들을 각각의 vertexshader pixelshader에 바인딩 시킨다.
	DEVICECONTEXT->VSSetShader(vertexShader.Get(), 0, 0);
	DEVICECONTEXT->PSSetShader(pixelShader.Get(), 0, 0);

	// 현재 rednertarget에 그려진
	DEVICECONTEXT->Draw(3, 0);

	Device::GetInst()->Present();
}
