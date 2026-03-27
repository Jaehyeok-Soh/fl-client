#pragma once
#include "json_forward.h"
#include "Engine_Utils.h"

NS_BEGIN(DTO)




#pragma region Citizen Gender Type
enum class CITIZEN_GENDERTYPE
{
	Male,
	Female,
	END,
};

static const char* g_szCitizenGenerType[(int)CITIZEN_GENDERTYPE::END] = {
	"Male",
	"Female",
};

inline std::string CitizenGenderType_ToString(CITIZEN_GENDERTYPE eType)
{
	if (eType >= CITIZEN_GENDERTYPE::Male && eType < CITIZEN_GENDERTYPE::END)
		return g_szCitizenGenerType[(int)eType];

	return "Unknown";
}

inline CITIZEN_GENDERTYPE CitizenGenderType_ToEnum(const std::string& strType)
{
	for (int i = 0; i < (int)CITIZEN_GENDERTYPE::END; ++i)
	{
		if (strType == g_szCitizenGenerType[i])
			return (CITIZEN_GENDERTYPE)i;
	}
	return CITIZEN_GENDERTYPE::END;
}
#pragma endregion

#pragma region Citizen PartType
enum class CITIZEN_PARTTYPE
{
	Hair,
	Beard,
	END,
};

static const char* g_szCitizenPartType[(int)CITIZEN_PARTTYPE::END] = {
	"Hair",
	"Beard",
};

inline std::string CitizenPartType_ToString(CITIZEN_PARTTYPE eType)
{
	if (eType >= CITIZEN_PARTTYPE::Hair && eType < CITIZEN_PARTTYPE::END)
		return g_szCitizenPartType[(int)eType];

	return "Unknown";
}

inline CITIZEN_PARTTYPE CitizenPartType_ToEnum(const std::string& strType)
{
	for (int i = 0; i < (int)CITIZEN_PARTTYPE::END; ++i)
	{
		if (strType == g_szCitizenPartType[i])
			return (CITIZEN_PARTTYPE)i;
	}
	return CITIZEN_PARTTYPE::END;
}
#pragma endregion

#pragma region Citizen Type
enum class CITIZEN_TYPE
{
	Child,
	Youth,
	Middle,
	Old,
	END,
};

static const char* g_szNpcCitizenType[(int)CITIZEN_TYPE::END] = {
	"Child",
	"Youth",
	"Middle",
	"Old"
};

inline std::string CitizenType_ToString(CITIZEN_TYPE eType)
{
	if (eType >= CITIZEN_TYPE::Child && eType < CITIZEN_TYPE::END)
		return g_szNpcCitizenType[(int)eType];

	return "Unknown";
}

inline CITIZEN_TYPE CitizenType_ToEnum(const std::string& strType)
{
	for (int i = 0; i < (int)CITIZEN_TYPE::END; ++i)
	{
		if (strType == g_szNpcCitizenType[i])
			return (CITIZEN_TYPE)i;
	}
	return CITIZEN_TYPE::END;
}
#pragma endregion

enum class CITIZEN_ATLAS_TYPE
{
	Eye,
	Mouth,
	END,
};


static std::string CITIZEN_ATLAS_TYPE_ToString(CITIZEN_ATLAS_TYPE eType)
{
	switch (eType)
	{
	case CITIZEN_ATLAS_TYPE::Eye:		return "Eye";
	case CITIZEN_ATLAS_TYPE::Mouth:		return "Mouth";
	default:							return "Unknonw";
	}

	return "Unknonw";
}

#pragma region Part Offset
static const Vec3 g_CitizenPartOffsetTable[ENUM_TO_UINT(CITIZEN_TYPE::END)][ENUM_TO_UINT(CITIZEN_GENDERTYPE::END)][ENUM_TO_UINT(CITIZEN_PARTTYPE::END)] =
{
	// ==========================================
	// 0. 유년 (Child)
	// ==========================================
	{
		// [Male]
		{
				{ 0.0f, -0.020f, 1.240f }, // Hair
				{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
		},
	// [Female]
	{
		{ 0.0f, -0.020f, 1.240f }, // Hair
		{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
	}
},

// ==========================================
// 1. 청년 (Youth)
// ==========================================
{
	// [Male]
	{
		{ 0.0f, -0.025f, 1.530f }, // Hair
		{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
	},
	// [Female]
	{
		{ 0.0f, -0.028f, 1.525f }, // Hair
		{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
	}
},

// ==========================================
// 2. 중년 (Middle)
// ==========================================
{
	// [Male]
	{
		{ 0.0f, -0.006f, 1.830f }, // Hair (6번 머리 탈락)
		{ 0.0f, -0.027f, 1.815f }  // Beard
	},
	// [Female]
	{
		{ 0.0f, -0.028f, 1.670f }, // Hair (2번 머리 탈락)
		{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
	}
},

// ==========================================
// 3. 노년 (Old)
// ==========================================
{
	// [Male]
	{
		{ 0.0f, -0.019f, 1.633f }, // Hair (할배)
		{ 0.0f, -0.035f, 1.625f }  // Beard
	},
	// [Female]
	{
		{ 0.0f, -0.080f, 1.560f }, // Hair (할매)
		{ 0.0f,  0.0f,   0.0f }    // Beard (없음)
	}
}
};

static Vec3 GetCitizenPartOffset(CITIZEN_TYPE eAge, CITIZEN_GENDERTYPE eGender, CITIZEN_PARTTYPE ePart)
{
	if (eAge < CITIZEN_TYPE::Child || eAge >= CITIZEN_TYPE::END ||
		eGender < CITIZEN_GENDERTYPE::Male || eGender >= CITIZEN_GENDERTYPE::END ||
		ePart < CITIZEN_PARTTYPE::Hair || ePart >= CITIZEN_PARTTYPE::END)
	{
		return { 0.0f, 0.0f, 0.0f };
	}

	if (eGender == CITIZEN_GENDERTYPE::Female && ePart == CITIZEN_PARTTYPE::Beard)
	{
		return { 0.0f, 0.0f, 0.0f };
	}

	return g_CitizenPartOffsetTable[ENUM_TO_UINT(eAge)][ENUM_TO_UINT(eGender)][ENUM_TO_UINT(ePart)];
}



static Vec3 Get_CitizenPartsOffset_ByFolderPath(const wstring& wstrFolderPath)
{
	Vec3 vOut = { 0.0f, 0.0f, 0.0f }; // 혹은 Vec3::Zero
	if (wstrFolderPath.empty()) return vOut;

	path p(wstrFolderPath);

	// 경로 예시: C:\...\Old\Female\Hair\CM_LNF_Hair01_SM

	// 1. 파일/폴더 이름 (CM_LNF_Hair01_SM) -> 지금은 안 쓰지만 구조상 확인
	// wstring wstrFileName = p.filename().wstring();

	// 2. 파츠 타입 (Hair) 추출
	p = p.parent_path();
	std::string strPartType = Engine_Utils::ToString((p.filename().wstring()));

	// 3. 성별 (Female) 추출
	p = p.parent_path();
	std::string strGender = Engine_Utils::ToString((p.filename().wstring()));

	// 4. 나이 (Old) 추출
	p = p.parent_path();
	std::string strAge = Engine_Utils::ToString((p.filename().wstring()));

	// 5. 문자열을 수강생님이 만든 Enum으로 변환!
	CITIZEN_TYPE eAge = CitizenType_ToEnum(strAge);
	CITIZEN_GENDERTYPE eGender = CitizenGenderType_ToEnum(strGender);
	CITIZEN_PARTTYPE ePart = CitizenPartType_ToEnum(strPartType);

	// 6. Enum 변환이 실패했을 경우의 안전장치 (Unknown 반환 시)
	if (eAge == CITIZEN_TYPE::END || eGender == CITIZEN_GENDERTYPE::END || ePart == CITIZEN_PARTTYPE::END)
	{
		MSG_BOX(" Citizen Type이 정확하지 않습니다");
		return vOut;
	}

	// 7. 앞서 만든 완벽한 오프셋 룩업 함수 호출!
	return GetCitizenPartOffset(eAge, eGender, ePart);
}

// 정보를 담을 간단한 구조체
struct CitizenModelInfo {
	DTO::CITIZEN_TYPE eAge;
	DTO::CITIZEN_GENDERTYPE eGender;
};

// Body(Cloth) 경로를 파싱하여 Enum 타입으로 반환해 주는 함수
static CitizenModelInfo Get_CitizenInfo_ByClothPath(const wstring& wstrFolderPath)
{
	CitizenModelInfo info = { DTO::CITIZEN_TYPE::END, DTO::CITIZEN_GENDERTYPE::END };
	if (wstrFolderPath.empty()) return info;

	using namespace std::filesystem;
	path p(wstrFolderPath);

	// 경로 예시: C:\...\Old\Female\CM_LNF_Body01

	// 1. 성별 (Female) 추출
	p = p.parent_path();
	std::string strGender = Engine_Utils::ToString(p.filename().wstring());

	// 2. 나이 (Old) 추출
	p = p.parent_path();
	std::string strAge = Engine_Utils::ToString(p.filename().wstring());

	// 3. Enum 변환
	info.eAge = CitizenType_ToEnum(strAge);
	info.eGender = CitizenGenderType_ToEnum(strGender);

	return info;
}



struct CittzenFaceUV
{
	Vec2 vUVOffset; // 선택된 셀의 시작 UV (예: 0.25, 0.5)
	Vec2 vUVScale; // 한 셀의 가로세로 크기 (예: 1.0/MaxCol, 1.0/MaxRow)
};

struct ENGINE_DLL CB_CitizentFaceData
{
	CittzenFaceUV	tCitizenFaceUV[ENUM_TO_UINT(CITIZEN_ATLAS_TYPE::END)];
	// C++ 전용 편의 함수: 쉐이더에는 영향을 주지 않음
public:
	void SetFaceUV(CITIZEN_ATLAS_TYPE eType, const struct CITIZEN_ATLAS_DATA* pData);
};


struct CITIZEN_PART_DATA
{
	Vec4	vColor{ 1.f,1.f,1.f,1.f };
	string	strName{ "" };
public:
	void from_Json(const json& LoadJson);
	void to_Json(json& SaveJson);
};


struct CITIZEN_ATLAS_DATA
{
	_bool		isUseAtlas{ false };
	_uint		iMaxRow{};
	_uint		iMaxColumn{};

	_uint		iSelectRow{}; // 입 또는 눈 모양 Column=>추후 애니매이션을 위한것 단순 모양만 일단정하기
	_uint		iSelectColumn{};
public:
	CITIZEN_ATLAS_DATA()
		: isUseAtlas{ false }, iMaxRow{ 1 }, iMaxColumn{ 1 }, iSelectRow{ 0 }, iSelectColumn{0}
	{

	}
	CITIZEN_ATLAS_DATA(_bool isUse, _uint _iMaxRow, _uint  _iMaxColumn, _uint _iSelectRow , _uint _iSelectColumn)
		: isUseAtlas{ isUse }, iMaxRow{ _iMaxRow }, iMaxColumn{ _iMaxColumn }, iSelectRow{ _iSelectRow }
		, iSelectColumn{ _iSelectColumn }
	{

	}
public:
	void from_Json(const json& LoadJson);
	void to_Json(json& SaveJson);
};

struct CITIZEN_DATA
{
	std::array<CITIZEN_ATLAS_DATA, ENUM_TO_UINT(CITIZEN_ATLAS_TYPE::END)> arrayNpcAtlasData = { CITIZEN_ATLAS_DATA(true , 4 , 4 , 0, 0) , CITIZEN_ATLAS_DATA(true , 3 , 3 ,0,0) };
	array<CITIZEN_PART_DATA, ENUM_TO_UINT(DTO::CITIZEN_PARTTYPE::END)>	  arrayPartDatas;

	_bool isUseClothColorMapping;
	SHADER_RGBCOLOR_DESC tClothRGBColor;

	string				strModelName;
	string				strLoopAnimationName; /* 단순 Citizen NPC들이 계속 Loop할 애니매이션 목록 */

	/* 추후 이동을 하는 NPC Type 추가될 예정 */

public:
	CITIZEN_DATA()
		: arrayNpcAtlasData{ CITIZEN_ATLAS_DATA(true , 4 , 4 , 0 , 0) , CITIZEN_ATLAS_DATA(true , 3 , 3 ,0, 0) }
		, isUseClothColorMapping{ true }
		, tClothRGBColor{}
		, strLoopAnimationName{}
		, strModelName{ "" }
	{
		arrayPartDatas.fill(CITIZEN_PART_DATA{});
	}
	CITIZEN_DATA(const CITIZEN_DATA& rhs)
		: arrayNpcAtlasData{ rhs.arrayNpcAtlasData }
		, isUseClothColorMapping{ rhs.isUseClothColorMapping }
		, tClothRGBColor{ rhs.tClothRGBColor }
		, strLoopAnimationName{ rhs.strLoopAnimationName }
		, strModelName{ rhs.strModelName }
		, arrayPartDatas{ rhs.arrayPartDatas }
	{

	}
	CITIZEN_DATA& operator=(const CITIZEN_DATA& rhs)
	{
		this->arrayNpcAtlasData = rhs.arrayNpcAtlasData;
		this->isUseClothColorMapping = rhs.isUseClothColorMapping;
		this->tClothRGBColor = rhs.tClothRGBColor;
		this->strLoopAnimationName = rhs.strLoopAnimationName;
		this->arrayPartDatas = rhs.arrayPartDatas;
		this->strModelName = rhs.strModelName;

		return *this;
	}
public:
	void Reset_PartsData()
	{
		this->arrayPartDatas.fill(CITIZEN_PART_DATA{});
	}
public:
	void from_Json(const json& LoadJson);
	void to_Json(json& SaveJson);
};


NS_END



