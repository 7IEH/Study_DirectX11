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
	// ��ġ �ʱ�ȭ
	D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION,
		m_Device.GetAddressOf(), &eLevel, m_Context.GetAddressOf())))
	{
		return E_FAIL;
	}

	// ���� ü�� ����
	if (FAILED(CreateSwapChain()))
	{
		return E_FAIL;
	}

	// ���� Ÿ�� ��
	if (FAILED(CreateTargetView()))
	{
		return E_FAIL;
	}

	// ViewPort ����
	// VIewPort�� ������ ���ϸ� ���� ����� ���� ���ϴ°�
	// TopLeft,Topright���� �ʺ�, ���̸�ŭ ���ȭ���� ������ �ش� �������� �����
	D3D11_VIEWPORT ViewportDesc = {};

	ViewportDesc.MinDepth = 0.f;
	ViewportDesc.MaxDepth = 1.f;
	ViewportDesc.TopLeftX = 0.f;
	ViewportDesc.TopLeftY = 0.f;

	ViewportDesc.Width = m_vRenderResolution.x;
	ViewportDesc.Height = m_vRenderResolution.y;

	// ����Ʈ -> clipspace(4���� ������ǥ��) -> NDC(cliping �� ���� + W ���ϱⰡ �� �� 3���� ���) -> ��ũ���� �����Ǵ� �������� ��ȯ�ϱ� ���� �ش� ����Ʈ�� 
	// resterization ���������� ���ε���
	m_Context->RSSetViewports(1, &ViewportDesc);

	return S_OK;
}

void Device::ClearRenderTarget(float(&Color)[4])
{
	m_Context->ClearRenderTargetView(m_RTView.Get(), Color);

	// ���� ���� 1�� ���� ������ ���� ���� ���� ���� ȭ�鿡 ����� �Ŷ�� �����ϸ�ȴ�.
	// �� 0���� ������ 0���� �� ���� ��ü�� �ִ� ���� ���� ������ �ƹ��͵� ǥ���� �� ����
	m_Context->ClearDepthStencilView(m_DSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Device::Present()
{
	m_SwapChain->Present(0, 0);
}

int Device::CreateSwapChain()
{
	// SwapChain ���� ����ü
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	// SwapChain �� �����ϴ� Buffer(RenderTarget) �� ���� ����
	tDesc.BufferCount = 1;
	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tDesc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
	tDesc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // buffer�� pixle format
	tDesc.BufferDesc.RefreshRate.Denominator = 1; // �и� 1
	tDesc.BufferDesc.RefreshRate.Numerator = 60; // ���� 60 
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tDesc.Flags = 0;

	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;

	tDesc.Windowed = true; // â���
	tDesc.OutputWindow = m_hRenderWnd; // swapchain ��� ������ ����

	// swapchain�� ������Ű�� ��ü ����
	IDXGIDevice* pIdxgiDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIdxgiDevice);
	pIdxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	pIdxgiDevice->Release();
	pAdapter->Release();
	pFactory->Release();


	// ���� ü�� ����
	if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &tDesc, m_SwapChain.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int Device::CreateTargetView()
{
	// ����Ÿ�� �ؽ��ĸ� ����ü������ ���� ���´�.
	// ���� ü���� ���� ���۸� ��������( ���� ü���� �����ϴ� ���� �ؽ��İ� ������(������ DC�� ������ bitmap�̶� �����ϸ�� )
	// �ش� ���۴� GPU�ȿ� �ִ� �޸𸮿� ����� ���� API ������Ʈ ���� ��� RAM�� ����Ǿ��µ�
	// GPU �� �����ϴ� ���ҽ��� �������ִ� ������ ��
	// ID3D11Resource(�θ� Ŭ����) �ڽ� - > buffer , texture2d ���� ����
	// swapchain���� ���� ������� buffer resource�� ����Ű�� ���� �����
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_RTTex.GetAddressOf())))
	{
		return E_FAIL;
	}

	// RenderTargetView
	// DepthStencilView texture�� ����
	// ShaderResourceView 
	// UnorderedAccessView

	// ���ҽ� ��ü�� �̿��ؼ� �߰��� view�� ������ ���ҽ��� �ʿ��� ���� view�� �޶���
	// ���� : �ؽ��͸� �ϳ� ���鶧�� �뵵�� ���������� ��, �� �뵵�� �´� view�� �޶��ؾ� �� �뵵�� �°� ������ �� ����
	// ������ �ش� ���ҽ� ��� �� ���ҽ��� �ۿ� �������� ���ϱ� ����
	// ������ ���ܷ� �뵵�� �´� �並 ���� �� �ִ� ���ҽ� ��ü�� ����
	// rendertargettview : �׸��� �׷��� ��(rendertarget texture�� ����Ű�� ����)
	// shaderview : �����س��� �׸��� �׸����� ������ ����
	// �ظ��ϸ� �ش� �Լ��� ������ ���� ���� �� ���� texture�� ������ �� ���� ������ 
	// �� ���� �Լ� ���� swapchain�� ������ ���� �ش� �Լ��� ������ �� ��찡 ����
	if (FAILED(m_Device->CreateRenderTargetView(m_RTTex.Get(), nullptr, m_RTView.GetAddressOf())))
	{
		return E_FAIL;
	}

	// DepthStencilTexture
	// pixel �ϳ��� 4����Ʈ
	// 4����Ʈ�� ���̸� ������ ��
	// 1����Ʈ�� stencil ������ ������ ��
	D3D11_TEXTURE2D_DESC Desc = {};

	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// DepthStencilState �ؽ��� �ػ󵵴� �ݵ�� RenderTargetTexture�� �����ؾ��Ѵ�.
	Desc.Width = (UINT)m_vRenderResolution.x;
	Desc.Height = (UINT)m_vRenderResolution.y;

	// ���̸� �����ϴ� texture�ٶ�� �뵵�� �����ִ°�
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// GPU �޸𸮸� ����Ͽ� ��������ٰ� ������, 
	// CPU (�ָ޸�)�� ������ ���� flag
	Desc.CPUAccessFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	// ���ø��� ����
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	// ������Ƽ ������ �纻 ����
	// ������ �ػ󵵸� ���� ���ߴ°�
	// �Ÿ��� ���� �ػ󵵿� Ȱ����
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

	// ������ ������ ���� device�� Ȱ����

	// OM(Outpt Merge State) �� RenderTargetTexture �� DepthstencilTexture �� �����Ѵ�.
	m_Context->OMSetRenderTargets(1, m_RTView.GetAddressOf(), m_DSView.Get());

	// swapchain�� ������ �ִ� ���� ���ҽ� ����(��, swapchain�� ���� �� ���ҽ��� ���� ���� Ƚ���� 1�ø��� ����)
	// m_RTTex��� ������ ������ �ٽ� ������ ����Ƚ���� 1 �� �þ
	// �޸� ���� �����ϱ� ���ؼ� �ش� ���ҽ��� ������ m_RTTex�� �����ؾ���
	// ���� ī���͸� �׻� ������ üũ�ؾ���
	return S_OK;
}
