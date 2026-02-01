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


void Engine_Utils::Add_Text(OUT string& str_out, const string& strfind, const string& stradd, _bool isback, _int32 ioffset)
{

    if (strfind.empty()) return;

    size_t sztPos = isback ? str_out.rfind(strfind) : str_out.find(strfind);

    if (sztPos == string::npos) return;

    _int64 iOffset = ioffset == 0 ? strfind.length() : ioffset;

    size_t sztFinalPos = sztPos + iOffset;

    if (sztFinalPos <= str_out.length())
    {
        str_out.insert(sztFinalPos, stradd);
    }
}

void Engine_Utils::Add_Text(OUT wstring& wstr_out, const wstring& wstrfind, const wstring& wstradd, _bool isback, _int32 ioffset)
{
    if (wstrfind.empty()) return;

    size_t sztPos = isback ? wstr_out.rfind(wstrfind) : wstr_out.find(wstrfind);

    if (sztPos == wstring::npos) return;

    _int64 iOffset = ioffset == 0 ? wstrfind.length() : ioffset;



    size_t sztFinalPos = (_int64)sztPos + iOffset;

    if (sztFinalPos <= wstr_out.length())
    {
        wstr_out.insert(sztFinalPos, wstradd);
    }
}

string Engine_Utils::NormalizePath(const std::filesystem::path& path)
{
    std::string s = path.lexically_normal().string();  // ../, ./ 정리 + 문자열 변환
    std::replace(s.begin(), s.end(), '\\', '/');       // 윈도우 백슬래시 → 슬래시
    return s;
}

wstring Engine_Utils::NormalizePath_WString(const std::filesystem::path& path)
{
    std::wstring ws = path.lexically_normal().wstring();  // ../, ./ 정리 + 문자열 변환
    std::replace(ws.begin(), ws.end(), '\\', '/');       // 윈도우 백슬래시 → 슬래시
    return ws;
}



wstring Engine_Utils::ToWString(string value)
{
    if (value.empty())
        return wstring();

    _int iRequire = ::MultiByteToWideChar(
        CP_ACP, MB_ERR_INVALID_CHARS,
        value.data(),
        static_cast<_int>(value.size()),
        nullptr, 0);

    if (iRequire <= 0)
        return wstring();

    wstring wstrReturn(static_cast<size_t>(iRequire), L'\0');

    _int iWritten = ::MultiByteToWideChar(
        CP_ACP, MB_ERR_INVALID_CHARS,
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

Vec3 Engine_Utils::ToEulerDegrees(const Quat& q)
{
    Matrix matRot = Matrix::CreateFromQuaternion(q);

    Vec3 vEuler;

    // 행렬에서 각 성분을 추출합니다
    float asinVal = -matRot._32;
    if (asinVal < -1.0f) asinVal = -1.0f;
    if (asinVal > 1.0f) asinVal = 1.0f;

    vEuler.x = asin(asinVal); // Pitch

    if (cos(vEuler.x) > 0.0001f)
    {
        vEuler.y = atan2(matRot._31, matRot._33); // Yaw
        vEuler.z = atan2(matRot._12, matRot._22); // Roll
    }
    else
    {
        // 짐벌락(Gimbal Lock) 발생 시 처리
        vEuler.y = 0.0f;
        vEuler.z = atan2(-matRot._21, matRot._11);
    }

    return Vec3(DirectX::XMConvertToDegrees(vEuler.x),
        DirectX::XMConvertToDegrees(vEuler.y),
        DirectX::XMConvertToDegrees(vEuler.z));
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
