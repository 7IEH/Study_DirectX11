#pragma once
class Device
	:public Singleton<Device>
{
	Single(Device)
private:
	// Comptr�� ����Ƚ���� �������� �ڵ����� �޸𸮸� �������ش�.
	ComPtr<ID3D11Device>				m_Device; //GPU �޸� ����, ��ü ����
	ComPtr<ID3D11DeviceContext>			m_Context; // GPU ������ ���
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