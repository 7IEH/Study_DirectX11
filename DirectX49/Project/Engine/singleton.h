#pragma once

template<typename T>
class Singleton
{
private:
	static T* m_This;

public:
	static T* GetInst()
	{
		if (m_This == nullptr)
		{
			m_This = new T();
		}
		return m_This;
	}

	static void Destroy()
	{
		if (m_This != nullptr)
		{
			delete m_This;
			m_This = nullptr;
		}
	}
	
public:
	Singleton()
	{
		typedef void(*FUNC_TYPE)(void);
		atexit((FUNC_TYPE)(&Singleton::Destroy));
	}
	~Singleton()
	{

	}
};

template<typename T>
T* Singleton<T>::m_This = nullptr;
