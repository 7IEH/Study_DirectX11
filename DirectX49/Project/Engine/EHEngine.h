#pragma once

class Engine
	:public Singleton<Engine>
{
	Single(Engine)
private:
	POINT		m_vResolution;
	HWND		m_hmainhWnd;

public:
	int Init(HWND _hWnd,POINT _vResolution);
	void Progress();
};

