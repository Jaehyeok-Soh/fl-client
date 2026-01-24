#include "Engine_pch.h"
#include "Engine_Utils.h"
#include <fstream>
#include <filesystem>

_bool Engine_Utils::StartsWith(const string& str, const string& comp)
{
    string::size_type iIndex = str.find(comp);
    if ((iIndex != string::npos) && (iIndex == 0))
        return true;

    return false;
}

_bool Engine_Utils::StartsWith(const wstring& wstr, const wstring& comp)
{
    wstring::size_type iIndex = wstr.find(comp);
    if ((iIndex != wstring::npos) && (iIndex == 0))
        return true;

    return false;
}

void Engine_Utils::Replace(OUT string& str, string comp, string rep)
{
    string temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}

void Engine_Utils::Replace(OUT wstring& str, wstring comp, wstring rep)
{
    wstring temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}

wstring Engine_Utils::ToWString(string value)
{
    if (value.empty())
        return wstring();

    _int iRequire = ::MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        value.data(),
        static_cast<_int>(value.size()),
        nullptr, 0);

    if (iRequire <= 0)
        return wstring();

    wstring wstrReturn(static_cast<size_t>(iRequire), L'\0');

    _int iWritten = ::MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        value.data(), static_cast<_int>(value.size()),
        wstrReturn.data(), iRequire);

    if (iWritten <= 0)
        return wstring();

    return wstrReturn;
}

string Engine_Utils::ToString(wstring value)
{
    if (value.empty())
        return string();

    _int iRequire = ::WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        value.data(),
        static_cast<_int>(value.size()),
        nullptr, 0, nullptr, nullptr);

    if (iRequire <= 0)
        return string();

    string strReturn(static_cast<size_t>(iRequire), L'\0');

    _int iWritten = ::WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        value.data(), static_cast<_int>(value.size()),
        strReturn.data(), iRequire, nullptr, nullptr);

    if (iWritten <= 0)
        return string();

    return strReturn;
}


string Engine_Utils::GetFileNameFromPath(const string& filePath)
{
    std::filesystem::path path(filePath);

    return path.filename().string();
}

string Engine_Utils::GetFileNameWithoutExtension(const string& filePath)
{
    std::filesystem::path path(filePath);

    return path.stem().string();
}

void Engine_Utils::Add_Flag(Flags& curFlags, _uint iBitFlag)
{
    curFlags |= iBitFlag;
}

void Engine_Utils::RemoveSoft_Flag(Flags& curFlags, _uint iBitFlag)
{
    if (curFlags & iBitFlag)
        curFlags ^= iBitFlag;
}

void Engine_Utils::RemoveHard_Flag(Flags& curFlags, _uint iBitFlag)
{
    curFlags &= ~iBitFlag;
}

bool Engine_Utils::Has_Flag(Flags curFlags, _uint iBitFlag)
{
    return (curFlags & iBitFlag) != 0;
}

bool Engine_Utils::Has_OnlyFlag(Flags curFlags, _uint iCheckFlag)
{
    return (curFlags & iCheckFlag) && (curFlags == iCheckFlag);
}

void Engine_Utils::Toggle_Flag(Flags& curFlags, _uint iBitFlag)
{
    curFlags ^= iBitFlag;
}

void Engine_Utils::Set_OnlyFlag(Flags& curFlags, _uint iBitFlag)
{
    // 1. 단일 비트인지 검사 (예: 1,2,4,8,...)
    if (iBitFlag == 0 || (iBitFlag & (iBitFlag - 1)) != 0)
    {
        // 잘못된 플래그일 경우 무시
        return;
    }

    // 2. 모든 하위 비트 제거
    curFlags = 0;

    // 3. 해당 플래그만 켜기
    curFlags |= iBitFlag;
}
