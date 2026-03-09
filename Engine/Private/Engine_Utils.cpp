#include "Engine_pch.h"
#include "Engine_Utils.h"
#include <cctype>
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

wstring Engine_Utils::To_Lower(wstring s)
{
    for (auto& wch : s)
        wch = static_cast<_tchar>(::towlower(wch));
    return s;
}

wstring Engine_Utils::To_Upper(wstring s)
{
    for (auto& wch : s)
        wch = static_cast<_tchar>(::toupper(wch));
    return s;
}

_bool Engine_Utils::Has_Token(const wstring& stemUpper, const wstring& tokenUpper)
{
    size_t iPos = stemUpper.find(tokenUpper);
    while (iPos != std::wstring::npos)
    {
        size_t iEnd = iPos + tokenUpper.size();

        if (iEnd >= stemUpper.size() || stemUpper[iEnd] == L'_')
            return true;

        iPos = stemUpper.find(tokenUpper, iPos + 1);
    }

    return false;
}

wstring Engine_Utils::Normalize_PathKey(const path& filePath)
{
    path src = filePath.lexically_normal();
    wstring wstr = src.generic_wstring();
    return To_Lower(src);
}

void Engine_Utils::Hash_HasCombine(size_t& seed, size_t value)
{
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
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
    // 03/02 소재혁, 한글 추가로 수정

    if (value.empty())
        return wstring();

    auto TryConvert = [&](UINT codePage, DWORD flags) -> wstring
        {
            _int iRequire = ::MultiByteToWideChar(
                codePage, flags,
                value.data(),
                static_cast<_int>(value.size()),
                nullptr, 0);

            if (iRequire <= 0)
                return wstring();

            wstring wstrReturn(static_cast<size_t>(iRequire), L'\0');
            _int iWritten = ::MultiByteToWideChar(
                codePage, flags,
                value.data(), static_cast<_int>(value.size()),
                wstrReturn.data(), iRequire);

            if (iWritten <= 0)
                return wstring();

            return wstrReturn;
        };

    wstring result = TryConvert(CP_UTF8, MB_ERR_INVALID_CHARS);
    if (result.empty())
        result = TryConvert(CP_ACP, 0);

    return result;
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

void Engine_Utils::Merge_MinMax(const Vec3* pMinMax, Vec3& ioMin, Vec3& ioMax)
{
    const Vec3& vMin = pMinMax[0];
    const Vec3& vMax = pMinMax[1];

    ioMin.x = (std::min)(ioMin.x, vMin.x);
    ioMin.y = (std::min)(ioMin.y, vMin.y);
    ioMin.z = (std::min)(ioMin.z, vMin.z);

    ioMax.x = (std::max)(ioMax.x, vMax.x);
    ioMax.y = (std::max)(ioMax.y, vMax.y);
    ioMax.z = (std::max)(ioMax.z, vMax.z);
}

BoundingBox Engine_Utils::MakeAABB_FromMinMax(const Vec3& vMin, const Vec3& vMax)
{
    BoundingBox boundingBox;
    XMFLOAT3 vPoints[2] = { vMin, vMax };
    BoundingBox::CreateFromPoints(boundingBox, 2, vPoints, sizeof(XMFLOAT3));
    return boundingBox;
}

BoundingSphere Engine_Utils::MakeSphere_FromMinMax(const Vec3& vMin, const Vec3& vMax)
{
    BoundingSphere boundingSphere;
    XMFLOAT3 vPoints[2] = { vMin, vMax };
    BoundingSphere::CreateFromPoints(boundingSphere, 2, vPoints, sizeof(XMFLOAT3));
    return boundingSphere;
}

void Engine_Utils::read_vec3_xyz(const json& _j, Vec3& vOut)
{
    vOut.x = _j.value("X", 0.f);
    vOut.y = _j.value("Y", 0.f);
    vOut.z = _j.value("Z", 0.f);
}

void Engine_Utils::read_vec2_xy(const json& _j, Vec2& vOut)
{
    vOut.x = _j.value("X", 0.f);
    vOut.y = _j.value("Y", 0.f);
}

void Engine_Utils::read_vec3_PitchYawRoll(const json& _j, Vec3& vOut)
{
    vOut.x = _j.value("Pitch", 0.f);
    vOut.y = _j.value("Yaw", 0.f);
    vOut.z = _j.value("Roll", 0.f);
}

void Engine_Utils::read_vec4_Quat(const json& _j, Quat& vOut)
{
    vOut.x = _j.value("X", 0.f);
    vOut.y = _j.value("Y", 0.f);
    vOut.z = _j.value("Z", 0.f);
    vOut.w = _j.value("W", 0.f);
}

void Engine_Utils::read_vec4_xyzw(const json& _j, Vec4& vOut)
{
    vOut.x = _j.value("X", 0.f);
    vOut.y = _j.value("Y", 0.f);
    vOut.z = _j.value("Z", 0.f);
    vOut.w = _j.value("W", 0.f);
}

void Engine_Utils::write_vec3_xyz(json& _j, const Vec3& vOut)
{
    _j["X"] = vOut.x;
    _j["Y"] = vOut.y;
    _j["Z"] = vOut.z;
}

void Engine_Utils::write_vec2_xy(json& _j, const Vec2& vOut)
{
    _j["X"] = vOut.x;
    _j["Y"] = vOut.y;
}

void Engine_Utils::write_vec3_PitchYawRoll(json& _j, const Vec3& vOut)
{
    _j["Pitch"] = vOut.x;
    _j["Yaw"] = vOut.y;
    _j["Roll"] = vOut.z;
}

void Engine_Utils::write_vec4_Quat(json& _j,const  Quat& vOut)
{
    _j["X"] = vOut.x;
    _j["Y"] = vOut.y;
    _j["Z"] = vOut.z;
    _j["W"] = vOut.w;
}

void Engine_Utils::write_vec4_xyzw(json& _j, const Vec4& vOut)
{
    _j["X"] = vOut.x;
    _j["Y"] = vOut.y;
    _j["Z"] = vOut.z;
    _j["W"] = vOut.w;
}


string Engine_Utils::MaterialTextureType_ToString(EMaterialTextureType eType)
{
    switch (eType)
    {
    case EMaterialTextureType::DIFFUSE:           return "DIFFUSE";
    case EMaterialTextureType::SPECULAR:          return "SPECULAR";
    case EMaterialTextureType::AMBIENT:           return "AMBIENT";
    case EMaterialTextureType::EMISSIVE:          return "EMISSIVE";
    case EMaterialTextureType::HEIGHT:            return "HEIGHT";
    case EMaterialTextureType::NORMALS:           return "NORMALS";
    case EMaterialTextureType::SHININESS:         return "SHININESS";
    case EMaterialTextureType::OPACITY:           return "OPACITY";
    case EMaterialTextureType::DISPLACEMENT:      return "DISPLACEMENT";
    case EMaterialTextureType::LIGHTMAP:          return "LIGHTMAP";
    case EMaterialTextureType::REFLECTION:        return "REFLECTION";
    case EMaterialTextureType::BASE_COLOR:        return "BASE_COLOR";
    case EMaterialTextureType::NORMAL_CAMERA:     return "NORMAL_CAMERA";
    case EMaterialTextureType::EMISSION_COLOR:    return "EMISSION_COLOR";
    case EMaterialTextureType::METALNESS:         return "METALNESS";
    case EMaterialTextureType::DIFFUSE_ROUGHNESS: return "DIFFUSE_ROUGHNESS";
    case EMaterialTextureType::AMBIENT_OCCLUSION: return "AMBIENT_OCCLUSION";
    case EMaterialTextureType::UNKNOWN:           return "UNKNOWN";
    default:                                      return "NONE";
    }

}

EMaterialTextureType Engine_Utils::MaterialTextureType_ToEnum(string strType)
{
    if (strType == "DIFFUSE")           return EMaterialTextureType::DIFFUSE;
    if (strType == "SPECULAR")          return EMaterialTextureType::SPECULAR;
    if (strType == "AMBIENT")           return EMaterialTextureType::AMBIENT;
    if (strType == "EMISSIVE")          return EMaterialTextureType::EMISSIVE;
    if (strType == "HEIGHT")            return EMaterialTextureType::HEIGHT;
    if (strType == "NORMALS")           return EMaterialTextureType::NORMALS;
    if (strType == "SHININESS")         return EMaterialTextureType::SHININESS;
    if (strType == "OPACITY")           return EMaterialTextureType::OPACITY;
    if (strType == "DISPLACEMENT")      return EMaterialTextureType::DISPLACEMENT;
    if (strType == "LIGHTMAP")          return EMaterialTextureType::LIGHTMAP;
    if (strType == "REFLECTION")        return EMaterialTextureType::REFLECTION;
    if (strType == "BASE_COLOR")        return EMaterialTextureType::BASE_COLOR;
    if (strType == "NORMAL_CAMERA")     return EMaterialTextureType::NORMAL_CAMERA;
    if (strType == "EMISSION_COLOR")    return EMaterialTextureType::EMISSION_COLOR;
    if (strType == "METALNESS")         return EMaterialTextureType::METALNESS;
    if (strType == "DIFFUSE_ROUGHNESS") return EMaterialTextureType::DIFFUSE_ROUGHNESS;
    if (strType == "AMBIENT_OCCLUSION") return EMaterialTextureType::AMBIENT_OCCLUSION;

    return EMaterialTextureType::MAX_COUNT;
}