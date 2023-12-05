#include "pch.h"
#include "EHPathMgr.h"

PathMgr::PathMgr()
	: m_strPath{}
{

}

PathMgr::~PathMgr()
{

}

void PathMgr::Init()
{
	GetCurrentDirectoryW(255,m_strPath);

	size_t Len = wcslen(m_strPath);

	for (size_t i = Len - 1;i >= 0;--i)
	{
		if ('\\' == m_strPath[i])
		{
			m_strPath[i + 1] = '\0';
			break;
		}
	}

	Len = wcslen(m_strPath);

	for (size_t i = Len - 2;i >= 0;--i)
	{
		if ('\\' == m_strPath[i])
		{
			wchar_t check = m_strPath[i + 1];
			m_strPath[i + 1] = '\0';
			break;
		}
	}

	wcscat_s(m_strPath, 255, L"content\\");


	int a = 0;

}
