#include "FileUtils.h"

CFileUtils::CFileUtils()
{
}

HRESULT CFileUtils::Open(wstring wstrFilePath, FileMode eMode)
{
    if (eMode == FileMode::WRITE)
    {
        m_handle = ::CreateFile
        (
            wstrFilePath.c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
    }
    else if(eMode == FileMode::READ)
    {
        m_handle = ::CreateFile
        (
            wstrFilePath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
    }

    if (m_handle == INVALID_HANDLE_VALUE)
        return E_FAIL;

    return S_OK;
}

HRESULT CFileUtils::Write(void* pData, _uint iDataSize)
{
    if (!pData)
        return E_FAIL;

    _uint iNumOfBytes = 0;

    if (FALSE == ::WriteFile(m_handle, pData, iDataSize, (LPDWORD)&iNumOfBytes, nullptr))
        return E_FAIL;

    return S_OK;
}

HRESULT CFileUtils::WriteAllText(const string& data)
{
    if (m_handle == INVALID_HANDLE_VALUE)
        return E_FAIL;

    DWORD iWritten = { 0 };
    if (!::WriteFile(m_handle, data.data(), (DWORD)data.size(), &iWritten, nullptr))
        return E_FAIL;

    if (iWritten != data.size())
        return E_FAIL;

    return S_OK;
}

HRESULT CFileUtils::Write(const string& data)
{
    _uint iSize = (_uint)data.size();

    // 문자열 길이를 먼저 넣고 ( 사이즈를 먼저 넣고 )
    if (FAILED(Write(iSize)))
        return E_FAIL;

    // 사이즈 작성후 기입할 데이터가없으면 넘어가기
    if (iSize <= 0)
        return S_OK;

    // 문자열을 밀어 넣는다. ( 사이즈에 해당하는 데이터를 밀어 넣는다. )
    if (FAILED(Write((void*)data.data(), iSize)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFileUtils::Read(void** pData, _uint iDataSize)
{
    if (!pData)
        return E_FAIL;

    _uint iNumOfBytes = 0;
    if (FALSE == ::ReadFile(m_handle, *pData, iDataSize, (LPDWORD)&iNumOfBytes, nullptr))
        return E_FAIL;

    return S_OK;
}

HRESULT CFileUtils::Read(OUT string& data)
{
    _uint iSize = Read<_uint>();

    if (iSize <= 0)
        return S_OK;
    
    char* temp = new char[iSize + 1];
    temp[iSize] = 0;

    if (FAILED(Read((void**)&temp, iSize)))
        return E_FAIL;

    data = temp;
    Safe_Delete_Array(temp);

    return S_OK;
}

HRESULT CFileUtils::ReadAllText(OUT string& data)
{
    if (m_handle == INVALID_HANDLE_VALUE)
        return E_FAIL;
    
    DWORD iFileSize = ::GetFileSize(m_handle, nullptr);
    if (iFileSize == INVALID_FILE_SIZE)
        return E_FAIL;

    if (iFileSize == 0)
    {
        data.clear();
        return S_OK;
    }

    std::string tmp;
    tmp.resize(iFileSize);

    DWORD iReadBytes = { 0 };
    if (!::ReadFile(m_handle, tmp.data(), iFileSize, &iReadBytes, nullptr))
        return E_FAIL;

    if (iReadBytes != iFileSize)
        return E_FAIL;

    data.swap(tmp);
    return S_OK;
}

CFileUtils* CFileUtils::Create()
{
    return new CFileUtils;
}

void CFileUtils::Free()
{
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
    Super::Free();
}
