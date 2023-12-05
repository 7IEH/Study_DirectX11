#include "pch.h"
#include "EHDevice.h"

Device::Device()
	: m_Context(nullptr)
	, m_Device(nullptr)
	, m_SwapChain(nullptr)
	, m_RTTex(nullptr)
	, m_vRenderResolution({ 0,0 })
	, m_hRenderWnd(nullptr)
{
}

Device::~Device()
{
}

int Device::Init(HWND _hWnd, POINT _vResolution)
{
	m_vRenderResolution = _vResolution;
	m_hRenderWnd = _hWnd;
	// 장치 초기화
	D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION,
		m_Device.GetAddressOf(), &eLevel, m_Context.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 스왑 체인 생성
	if (FAILED(CreateSwapChain()))
	{
		return E_FAIL;
	}

	// 렌더 타켓 뷰
	if (FAILED(CreateTargetView()))
	{
		return E_FAIL;
	}

	// ViewPort 설정
	// VIewPort는 간단히 말하면 내가 출력할 곳을 말하는거
	// TopLeft,Topright에서 너비, 높이만큼 출력화면을 가지고 해당 영역에서 출력함
	D3D11_VIEWPORT ViewportDesc = {};

	ViewportDesc.MinDepth = 0.f;
	ViewportDesc.MaxDepth = 1.f;
	ViewportDesc.TopLeftX = 0.f;
	ViewportDesc.TopLeftY = 0.f;

	ViewportDesc.Width = m_vRenderResolution.x;
	ViewportDesc.Height = m_vRenderResolution.y;

	// 뷰포트 -> clipspace(4차원 동차좌표계) -> NDC(cliping 된 공간 + W 곱하기가 된 곳 3차원 평면) -> 스크린에 대응되느 공간으로 변환하기 위해 해당 뷰포트를 
	// resterization 스테이지에 바인드함
	m_Context->RSSetViewports(1, &ViewportDesc);

	return S_OK;
}

void Device::ClearRenderTarget(float(&Color)[4])
{
	m_Context->ClearRenderTargetView(m_RTView.Get(), Color);

	// 깊이 값을 1로 놓는 이유는 깊이 값이 작을 수록 화면에 가까운 거라고 생각하면된다.
	// 즉 0으로 놓으면 0보다 더 깊은 물체에 있는 것은 없기 때문에 아무것도 표현할 수 없다
	m_Context->ClearDepthStencilView(m_DSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Device::Present()
{
	m_SwapChain->Present(0, 0);
}

int Device::CreateSwapChain()
{
	// SwapChain 생성 구조체
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	// SwapChain 이 관리하는 Buffer(RenderTarget) 의 구성 정보
	tDesc.BufferCount = 1;
	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tDesc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
	tDesc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // buffer에 pixle format
	tDesc.BufferDesc.RefreshRate.Denominator = 1; // 분모 1
	tDesc.BufferDesc.RefreshRate.Numerator = 60; // 분자 60 
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tDesc.Flags = 0;

	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;

	tDesc.Windowed = true; // 창모드
	tDesc.OutputWindow = m_hRenderWnd; // swapchain 출력 윈도우 지정

	// swapchain을 생성시키는 객체 생성
	IDXGIDevice* pIdxgiDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIdxgiDevice);
	pIdxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	pIdxgiDevice->Release();
	pAdapter->Release();
	pFactory->Release();


	// 스왑 체인 생성
	if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &tDesc, m_SwapChain.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int Device::CreateTargetView()
{
	// 렌더타켓 텍스쳐를 스왑체인으로 부터 얻어온다.
	// 스왑 체인을 보면 버퍼를 생성했음( 스왑 체인이 관리하는 렌더 텍스쳐가 생성됨(옛날에 DC로 생성한 bitmap이라 생각하면됨 )
	// 해당 버퍼는 GPU안에 있는 메모리에 적재됨 예전 API 프로젝트 같은 경우 RAM에 저장되었는데
	// GPU 상에 존재하는 리소스를 관리해주는 역할을 함
	// ID3D11Resource(부모 클래스) 자식 - > buffer , texture2d 등이 있음
	// swapchain으로 부터 만들어진 buffer resource를 가리키기 위해 사용함
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_RTTex.GetAddressOf())))
	{
		return E_FAIL;
	}

	// RenderTargetView
	// DepthStencilView texture는 없음
	// ShaderResourceView 
	// UnorderedAccessView

	// 리소스 객체를 이용해서 중간에 view가 생성됨 리소스가 필요한 곳은 view를 달라함
	// 이유 : 텍스터를 하나 만들때도 용도가 정해져있음 즉, 그 용도에 맞는 view를 달라해야 그 용도에 맞게 가져올 수 있음
	// 이유는 해당 리소스 뷰는 그 리소스로 밖에 생성하지 못하기 때문
	// 하지만 예외로 용도에 맞는 뷰를 만들 수 있는 리소스 객체도 있음
	// rendertargettview : 그림이 그려질 곳(rendertarget texture를 가리키고 있음)
	// shaderview : 저장해놓고 그림을 그릴곳에 복사할 원본
	// 왠만하면 해당 함수는 실패할 일이 없음 그 전에 texture가 오류가 날 수는 있지만 
	// 그 전에 함수 역시 swapchain이 오류를 내지 해당 함수는 오류를 낼 경우가 없음
	if (FAILED(m_Device->CreateRenderTargetView(m_RTTex.Get(), nullptr, m_RTView.GetAddressOf())))
	{
		return E_FAIL;
	}

	// DepthStencilTexture
	// pixel 하나당 4바이트
	// 4바이트는 깊이를 저장한 것
	// 1바이트는 stencil 정보를 저장한 것
	D3D11_TEXTURE2D_DESC Desc = {};

	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// DepthStencilState 텍스쳐 해상도는 반드시 RenderTargetTexture와 동일해야한다.
	Desc.Width = (UINT)m_vRenderResolution.x;
	Desc.Height = (UINT)m_vRenderResolution.y;

	// 깊이를 저장하는 texture다라고 용도를 정해주는것
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// GPU 메모리를 사용하여 만들어진다고 했으나, 
	// CPU (주메모리)와 연락을 위한 flag
	Desc.CPUAccessFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	// 샘플링은 고정
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	// 저퀄리티 버전의 사본 샘플
	// 원본에 해상도를 점점 낮추는것
	// 거리에 따른 해상도에 활용함
	Desc.MipLevels = 1;
	Desc.MiscFlags = 0;

	Desc.ArraySize = 1;
	if (FAILED(m_Device->CreateTexture2D(&Desc, nullptr, m_DSTex.GetAddressOf())))
	{
		return E_FAIL;
	}
	//	DepthStencilView 

	if (FAILED(m_Device->CreateDepthStencilView(m_DSTex.Get(), nullptr, m_DSView.GetAddressOf())))
	{

		return E_FAIL;
	}

	// 뭔가를 생성할 때는 device를 활용함

	// OM(Outpt Merge State) 에 RenderTargetTexture 와 DepthstencilTexture 를 전달한다.
	m_Context->OMSetRenderTargets(1, m_RTView.GetAddressOf(), m_DSView.Get());

	// swapchain이 가지고 있던 렌더 리소스 였음(즉, swapchain이 현재 그 리소스에 대한 참조 횟수를 1늘리고 있음)
	// m_RTTex라는 포인터 변수로 다시 가리켜 참조횟수가 1 더 늘어남
	// 메모리 릭을 방지하기 위해서 해당 리소스를 참조한 m_RTTex를 해제해야함
	// 참조 카운터를 항상 유심히 체크해야함
	return S_OK;
}
