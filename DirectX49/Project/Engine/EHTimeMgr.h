#pragma once
class TimeMgr
	:public Singleton<TimeMgr>
{
	Single(TimeMgr)
private:
	float m_fDeltaTime;

public:
	void Init();
	void Progress();

public:
	float GetDelatTime() { return m_fDeltaTime; }
};

