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
	static wstring To_Upper(wstring s);
	static _bool Has_Token(const wstring& stemUpper, const wstring& tokenUpper);
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
	static _float EvalEase_SmoothStep(_float fT) { fT = std::clamp(fT, 0.f, 1.f); return fT * fT * (3.f - 2.f * fT); }
	static _float EvalEase_EaseOutQuad(_float fT) { fT = std::clamp(fT, 0.f, 1.f); return 1.f - (1.f - fT) * (1.f - fT); }
	static _float EvalEase_EaseInOutQuad(_float fT)
	{
		fT = std::clamp(fT, 0.f, 1.f);
		return (fT < 0.5f)
			? 2.f * fT * fT
			: 1.f - std::pow(-2.f * fT + 2.f, 2.f) * 0.5f;
	}
	static _float EvalEase_EaseInOutSine(_float fT)
	{
		fT = std::clamp(fT, 0.f, 1.f);
		return -(std::cos(XM_PI * fT) - 1.f) / 2.f;
	}

	static _float EvalEase_EaseInOutCubic(_float fT)
	{
		fT = std::clamp(fT, 0.f, 1.f);
		return (fT < 0.5f)
			? 4.f * fT * fT * fT
			: 1.f - std::pow(-2.f * fT + 2.f, 3.f) * 0.5f;
	}
	static _float EvalEase_EaseOutBack(_float fT)
	{
		fT = std::clamp(fT, 0.f, 1.f);

		constexpr _float fC1 = 1.70158f;
		constexpr _float fC3 = fC1 + 1.f;

		return 1.f + fC3 * std::pow(fT - 1.f, 3.f) + fC1 * std::pow(fT - 1.f, 2.f);
	}
	static _float EvalTimingWeight(_float fElapsed, _float fBlendIn, _float fHold, _float fBlendOut);
	
	static void Merge_MinMax(const Vec3* pMinMax, Vec3& ioMin, Vec3& ioMax);

	static BoundingBox MakeAABB_FromMinMax(const Vec3 &vMin, const Vec3 &vMax);
	static BoundingSphere MakeSphere_FromMinMax(const Vec3& vMin, const Vec3& vMax);

#pragma region Read Write Json

	static void read_vec3_xyz(const json& _j, Vec3& vOut);
	static void read_vec2_xy(const json& _j, Vec2& vOut);
	static void read_vec3_PitchYawRoll(const json& _j, Vec3& vOut);
	static void read_vec4_Quat(const json& _j, Quat& vOut);
	static void read_vec4_xyzw(const json& _j, Vec4& vOut);

	static void write_vec3_xyz(json& _j, const Vec3& vOut);
	static void write_vec2_xy(json& _j, const Vec2& vOut);
	static void write_vec3_PitchYawRoll(json& _j, const Vec3& vOut);
	static void write_vec4_Quat( json& _j, const Quat& vOut);
	static void write_vec4_xyzw( json& _j, const Vec4& vOut);

	// 문자를 숫자로 바꿔주는 해쉬 함수.
	constexpr static _uint ToHash(const char* str, _uint h = 2166136261u)
	{
		return !*str ? h : ToHash(str + 1, (h ^ _uint(*str)) * 16777619u);
	}

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
	static string				LIGHTTYPE_ToString(LIGHT_TYPE eType);
	static LIGHT_TYPE			LIGHTTYPE_ToEnum(const string& strType);
	static string				MaterialTextureType_ToString(EMaterialTextureType eType);
	static EMaterialTextureType MaterialTextureType_ToEnum(string strType);


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
		case Engine::EMaterialInstanceType::Free:
			return "MaterialInstance_Free";
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
		case Engine::EMaterialInstanceType::Free:
			return L"MaterialInstance_Free";
		default:
			return L"";
		}
	}

	static EMaterialInstanceType MI_ToEnum(const string& strType)
	{
		if (strType == "MaterialInstance_Default") return Engine::EMaterialInstanceType::Default;
		else if (strType == "MaterialInstance_Concrete") return Engine::EMaterialInstanceType::Concrete;
		else if (strType == "MaterialInstance_Mirror") return Engine::EMaterialInstanceType::Mirror;
		else if (strType == "MaterialInstance_Water") return Engine::EMaterialInstanceType::Water;
		else if (strType == "MaterialInstance_Dirt") return Engine::EMaterialInstanceType::Dirt;
		else if (strType == "MaterialInstance_Red") return Engine::EMaterialInstanceType::Red;
		else if (strType == "MaterialInstance_Blue") return Engine::EMaterialInstanceType::Blue;
		else if (strType == "MaterialInstance_Green") return Engine::EMaterialInstanceType::Green;
		else if (strType == "MaterialInstance_Grass") return Engine::EMaterialInstanceType::Grass;
		else if (strType == "MaterialInstance_Orange") return Engine::EMaterialInstanceType::Orange;
		else if (strType == "MaterialInstance_Brown") return Engine::EMaterialInstanceType::Brown;
		else if (strType == "MaterialInstance_Pupple") return Engine::EMaterialInstanceType::Pupple;
		else if (strType == "MaterialInstance_WinterGreen") return Engine::EMaterialInstanceType::WinterGreen;
		else if (strType == "MaterialInstance_BurnishedBrown") return Engine::EMaterialInstanceType::BurnishedBrown;
		else if (strType == "MaterialInstance_ConcreteLight") return Engine::EMaterialInstanceType::ConcreteLight;
		else if (strType == "MaterialInstance_ConcreteMid") return Engine::EMaterialInstanceType::ConcreteMid;
		else if (strType == "MaterialInstance_ConcreteWarm") return Engine::EMaterialInstanceType::ConcreteWarm;
		else if (strType == "MaterialInstance_MetalCool") return Engine::EMaterialInstanceType::MetalCool;
		else if (strType == "MaterialInstance_MetalDark") return Engine::EMaterialInstanceType::MetalDark;
		else if (strType == "MaterialInstance_CourtBlue") return Engine::EMaterialInstanceType::CourtBlue;
		else if (strType == "MaterialInstance_Free") return Engine::EMaterialInstanceType::Free;

		return Engine::EMaterialInstanceType::Default; // 기본값 반환
	}
	static EMaterialInstanceType MI_ToEnum(const wstring& wstrType)
	{
		if (wstrType == L"MaterialInstance_Default") return Engine::EMaterialInstanceType::Default;
		else if (wstrType == L"MaterialInstance_Concrete") return Engine::EMaterialInstanceType::Concrete;
		else if (wstrType == L"MaterialInstance_Mirror") return Engine::EMaterialInstanceType::Mirror;
		else if (wstrType == L"MaterialInstance_Water") return Engine::EMaterialInstanceType::Water;
		else if (wstrType == L"MaterialInstance_Dirt") return Engine::EMaterialInstanceType::Dirt;
		else if (wstrType == L"MaterialInstance_Red") return Engine::EMaterialInstanceType::Red;
		else if (wstrType == L"MaterialInstance_Blue") return Engine::EMaterialInstanceType::Blue;
		else if (wstrType == L"MaterialInstance_Green") return Engine::EMaterialInstanceType::Green;
		else if (wstrType == L"MaterialInstance_Grass") return Engine::EMaterialInstanceType::Grass;
		else if (wstrType == L"MaterialInstance_Orange") return Engine::EMaterialInstanceType::Orange;
		else if (wstrType == L"MaterialInstance_Brown") return Engine::EMaterialInstanceType::Brown;
		else if (wstrType == L"MaterialInstance_Pupple") return Engine::EMaterialInstanceType::Pupple;
		else if (wstrType == L"MaterialInstance_WinterGreen") return Engine::EMaterialInstanceType::WinterGreen;
		else if (wstrType == L"MaterialInstance_BurnishedBrown") return Engine::EMaterialInstanceType::BurnishedBrown;
		else if (wstrType == L"MaterialInstance_ConcreteLight") return Engine::EMaterialInstanceType::ConcreteLight;
		else if (wstrType == L"MaterialInstance_ConcreteMid") return Engine::EMaterialInstanceType::ConcreteMid;
		else if (wstrType == L"MaterialInstance_ConcreteWarm") return Engine::EMaterialInstanceType::ConcreteWarm;
		else if (wstrType == L"MaterialInstance_MetalCool") return Engine::EMaterialInstanceType::MetalCool;
		else if (wstrType == L"MaterialInstance_MetalDark") return Engine::EMaterialInstanceType::MetalDark;
		else if (wstrType == L"MaterialInstance_CourtBlue") return Engine::EMaterialInstanceType::CourtBlue;
		else if (wstrType == L"MaterialInstance_Free") return Engine::EMaterialInstanceType::Free;

		return Engine::EMaterialInstanceType::Default;
	}


#pragma region ObjectTag Enum Sring Change


#pragma endregion


#pragma region

	// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
	static inline const char* g_szLerpTypes[(int)Engine::ELerpType::END] = {
		"NONE",
		"Linear",
		"SlowStart",
		"SlowEnd",
		"SmoothStep",
		"Curve"
	};
	static inline string LerpType_ToString(ELerpType eType)
	{
		// 인덱스 초과 방지 안전장치
		if (eType >= Engine::ELerpType::NONE && eType < Engine::ELerpType::END)
			return g_szLerpTypes[(int)eType];

		return "Unknown";
	}

	static inline ELerpType LerpType_ToEnum(const string& strType)
	{
		for (int i = 0; i < (int)Engine::ELerpType::END; ++i)
		{
			if (strType == g_szLerpTypes[i])
				return (Engine::ELerpType)i;
		}
		return Engine::ELerpType::NONE;
	}

#pragma endregion


};

inline physx::PxVec3 ToPxVec3(const DirectX::SimpleMath::Vector3& v)
{
	return physx::PxVec3(v.x, v.y, v.z);
}

inline DirectX::SimpleMath::Vector3 ToVector3(const physx::PxVec3& v)
{
	return DirectX::SimpleMath::Vector3(v.x, v.y, v.z);
}

inline physx::PxQuat ToPxQuat(const DirectX::SimpleMath::Quaternion& v)
{
	return physx::PxQuat(v.x, v.y, v.z, v.w);
}

inline DirectX::SimpleMath::Quaternion ToQuaternion(const physx::PxQuat& v)
{
	return DirectX::SimpleMath::Quaternion(v.x, v.y, v.z, v.w);
}

NS_END