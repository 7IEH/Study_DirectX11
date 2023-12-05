#pragma once
class Device
	:public Singleton<Device>
{
	Single(Device)
private:
	// Comptr은 참조횟수를 바탕으로 자동으로 메모리를 해제해준다.
	ComPtr<ID3D11Device>				m_Device; //GPU 메모리 관리, 객체 생성
	ComPtr<ID3D11DeviceContext>			m_Context; // GPU 렌더링 명령
	ComPtr<IDXGISwapChain>				m_SwapChain;
	ComPtr<ID3D11Texture2D>				m_RTTex;
	ComPtr<ID3D11RenderTargetView>		m_RTView;

	ComPtr< ID3D11Texture2D>			m_DSTex;
	ComPtr<ID3D11DepthStencilView>		m_DSView;

	POINT								m_vRenderResolution;
	HWND								m_hRenderWnd;

public:
	int Init(HWND _hWnd, POINT _vResolution);
	void Present();
	void ClearRenderTarget(float(&Color)[4]);

	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return m_Context.Get(); }

private:
	int CreateSwapChain();
	int CreateTargetView();
};