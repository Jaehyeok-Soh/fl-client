#pragma once
#include "Base.h"

NS_BEGIN(Engine)

enum class FileMode : unsigned char
{
	WRITE = 0,
	READ
};

class ENGINE_DLL CFileUtils : public CBase
{
	using Super = CBase;
private:
	CFileUtils();
	virtual ~CFileUtils() = default;

	HRESULT Initialize();
public:
	HRESULT Open(wstring wstrFilePath, FileMode eMode);

	template<typename T>
	HRESULT Write(const T& data)
	{
		DWORD iNumOfBytes = 0;
		if (FALSE == ::WriteFile(m_handle, &data, sizeof(T), (LPDWORD)&iNumOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}

	template<>
	HRESULT Write<string>(const string& data)
	{
		return Write(data);
	}
	
	HRESULT Write(void* pData, _uint iDataSize);
	HRESULT Write(const string& data);
	HRESULT WriteAllText(const string &data);

	template<typename T>
	HRESULT Read(OUT T& data)
	{
		DWORD iNumOfBytes = 0;
		if (FALSE == ::ReadFile(m_handle, &data, sizeof(T), (LPDWORD)&iNumOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}
	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}
	HRESULT Read(void** pData, _uint iDataSize);
	HRESULT Read(OUT string& data);
	HRESULT ReadAllText(OUT string &data);
private:
	HANDLE m_handle = INVALID_HANDLE_VALUE;
public:
	static CFileUtils* Create();
	virtual void Free() override;
};

NS_END