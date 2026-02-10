#pragma once
#include "Engine_Define.h"
#include "json_forward.h"

NS_BEGIN(Engine)
 
class ENGINE_DLL Engine_Utils
{
public:
	static _bool StartsWith(const string& str, const string& comp);
	static _bool StartsWith(const wstring& wstr, const wstring& comp);

	static void Replace(OUT string& str, string comp, string rep);
	static void Replace(OUT wstring& str, wstring comp, wstring rep);

	static void Add_Text(OUT string& str_out, const string& strfind , const string& stradd ,_bool isback = false , _int32 ioffset = 0);
	static void Add_Text(OUT wstring& str_out, const wstring& strfind , const wstring& stradd , _bool isback = false, _int32 i0ffset = 0);

	static wstring To_Lower(wstring s);
	static wstring Normalize_PathKey(const path& filePath);
	static void Hash_HasCombine(size_t& seed, size_t value);

	static string NormalizePath(const std::filesystem::path& path);
	static wstring NormalizePath_WString(const std::filesystem::path& path);

	static wstring ToWString(string value);
	static string ToString(wstring value);

	static string GetFileNameFromPath(const string& filePath);
	static string GetFileNameWithoutExtension(const string& path);

	static string Make_DataFileKey(const std::filesystem::path& filePath) { return filePath.stem().string(); }
	// 쿼터니언 값을 오일러 값으로 바꿔서 반환해주는 함수
	static Vec3 ToEulerDegrees(const Quat& q);
	/* 비트 플래그 유틸 함수 */
	static void Add_Flag(Flags& curFlags, _uint iBitFlag);
	static void RemoveSoft_Flag(Flags& curFlags, _uint iBitFlag); // 있다면 지워라
	static void RemoveHard_Flag(Flags& curFlags, _uint iBitFlag); // 무조건 지우기
	static bool Has_Flag(Flags curFlags, _uint iBitFlag);
	static bool Has_OnlyFlag(Flags curFlags, _uint iCheckFlag);
	static void Toggle_Flag(Flags& curFlags, _uint iBitFlag);
	static void Set_OnlyFlag(Flags& curFlags, _uint iBitFlag);

	static _float SmoothStep(_float fT) { return fT * fT * (3.f - 2.f * fT); }

	static void Merge_MinMax(const Vec3* pMinMax, Vec3& ioMin, Vec3& ioMax);

	static BoundingBox MakeAABB_FromMinMax(const Vec3 &vMin, const Vec3 &vMax);
	static BoundingSphere MakeSphere_FromMinMax(const Vec3& vMin, const Vec3& vMax);

#pragma region Read Write Json

	static void read_vec3_xyz(const json& _j, Vec3& vOut);
	static void read_vec3_PitchYawRoll(const json& _j, Vec3& vOut);
	static void read_vec4_Quat(const json& _j, Quat& vOut);

	static void write_vec3_xyz(json& _j, const Vec3& vOut);
	static void write_vec3_PitchYawRoll(json& _j, const Vec3& vOut);
	static void write_vec4_Quat( json& _j, const Quat& vOut);


	static inline string	D3D11_USAGE_ToString(D3D11_USAGE eUsage)
	{
		switch (eUsage)
		{
		case D3D11_USAGE_DEFAULT:	return "DEFAULT";
		case D3D11_USAGE_IMMUTABLE:	return "IMMUTABLE";
		case D3D11_USAGE_DYNAMIC:	return "DYNAMIC";
		case D3D11_USAGE_STAGING:	return "STAGING";
		default:					return "DEFAULT";
		}

		return "DEFAULT";
	}
	static inline D3D11_USAGE	D3D11_USAGE_ToEnum(string strUsage)
	{
		if (strUsage == "DEFAULT")		return D3D11_USAGE_DEFAULT;
		if (strUsage == "IMMUTABLE")	return D3D11_USAGE_IMMUTABLE;
		if (strUsage == "DYNAMIC")		return D3D11_USAGE_DYNAMIC;
		if (strUsage == "STAGING")		return D3D11_USAGE_STAGING;

		return D3D11_USAGE_DEFAULT;
	}


#pragma endregion




	static string MI_ToString(EMaterialInstanceType eType)
	{
		switch (eType)
		{
		case Engine::EMaterialInstanceType::Default:
			return "MaterialInstance_Default";
		case Engine::EMaterialInstanceType::Concrete:
			return "MaterialInstance_Concrete";
		case Engine::EMaterialInstanceType::Mirror:
			return "MaterialInstance_Mirror";
		case Engine::EMaterialInstanceType::Water:
			return "MaterialInstance_Water";
		case Engine::EMaterialInstanceType::Dirt:
			return "MaterialInstance_Dirt";
		case Engine::EMaterialInstanceType::Red:
			return "MaterialInstance_Red";
		case Engine::EMaterialInstanceType::Blue:
			return "MaterialInstance_Blue";
		case Engine::EMaterialInstanceType::Green:
			return "MaterialInstance_Green";
		case Engine::EMaterialInstanceType::Grass:
			return "MaterialInstance_Grass";
		case Engine::EMaterialInstanceType::Orange:
			return "MaterialInstance_Orange";
		case Engine::EMaterialInstanceType::Brown:
			return "MaterialInstance_Brown";
		case Engine::EMaterialInstanceType::Pupple:
			return "MaterialInstance_Pupple";
		case Engine::EMaterialInstanceType::WinterGreen:
			return "MaterialInstance_WinterGreen";
		case Engine::EMaterialInstanceType::BurnishedBrown:
			return "MaterialInstance_BurnishedBrown";
		case Engine::EMaterialInstanceType::ConcreteLight:
			return "MaterialInstance_ConcreteLight";
		case Engine::EMaterialInstanceType::ConcreteMid:
			return "MaterialInstance_ConcreteMid";
		case Engine::EMaterialInstanceType::ConcreteWarm:
			return "MaterialInstance_ConcreteWarm";
		case Engine::EMaterialInstanceType::MetalCool:
			return "MaterialInstance_MetalCool";
		case Engine::EMaterialInstanceType::MetalDark:
			return "MaterialInstance_MetalDark";
		case Engine::EMaterialInstanceType::CourtBlue:
			return "MaterialInstance_CourtBlue";
		default:
			return "";
		}
	}
	static wstring MI_ToWString(EMaterialInstanceType eType)
	{
		switch (eType)
		{
		case Engine::EMaterialInstanceType::Default:
			return L"MaterialInstance_Default";
		case Engine::EMaterialInstanceType::Concrete:
			return L"MaterialInstance_Concrete";
		case Engine::EMaterialInstanceType::Mirror:
			return L"MaterialInstance_Mirror";
		case Engine::EMaterialInstanceType::Water:
			return L"MaterialInstance_Water";
		case Engine::EMaterialInstanceType::Dirt:
			return L"MaterialInstance_Dirt";
		case Engine::EMaterialInstanceType::Red:
			return L"MaterialInstance_Red";
		case Engine::EMaterialInstanceType::Blue:
			return L"MaterialInstance_Blue";
		case Engine::EMaterialInstanceType::Green:
			return L"MaterialInstance_Green";
		case Engine::EMaterialInstanceType::Grass:
			return L"MaterialInstance_Grass";
		case Engine::EMaterialInstanceType::Orange:
			return L"MaterialInstance_Orange";
		case Engine::EMaterialInstanceType::Brown:
			return L"MaterialInstance_Brown";
		case Engine::EMaterialInstanceType::Pupple:
			return L"MaterialInstance_Pupple";
		case Engine::EMaterialInstanceType::WinterGreen:
			return L"MaterialInstance_WinterGreen";
		case Engine::EMaterialInstanceType::BurnishedBrown:
			return L"MaterialInstance_BurnishedBrown";
		case Engine::EMaterialInstanceType::ConcreteLight:
			return L"MaterialInstance_ConcreteLight";
		case Engine::EMaterialInstanceType::ConcreteMid:
			return L"MaterialInstance_ConcreteMid";
		case Engine::EMaterialInstanceType::ConcreteWarm:
			return L"MaterialInstance_ConcreteWarm";
		case Engine::EMaterialInstanceType::MetalCool:
			return L"MaterialInstance_MetalCool";
		case Engine::EMaterialInstanceType::MetalDark:
			return L"MaterialInstance_MetalDark";
		case Engine::EMaterialInstanceType::CourtBlue:
			return L"MaterialInstance_CourtBlue";
		default:
			return L"";
		}
	}
};

NS_END