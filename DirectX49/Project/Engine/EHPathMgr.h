#pragma once
class PathMgr
	:public Singleton<PathMgr>
{
	Single(PathMgr)
private:

public:
	void Init();

	wchar_t* GetPath() { return m_strPath; }
public:
	wchar_t m_strPath[255];
};

