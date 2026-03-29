#pragma once
#include "json_forward.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class CShader;
class CModel;

NS_END


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

struct CitizenWalkRunAnimIndex
{
	_uint iWalkAnimIndex{};
	_uint iRunAnimIndex{};
};

struct ENGINE_DLL CitizenWalkRunAnimIndexData
{
private:
	/* Floder Name을 Hash 값으로해서 보관하자 Preset 데이터에서 뜯은 Hash값으로 */
	static inline std::map<_uint,CitizenWalkRunAnimIndex>  mapCitizenWalkRunAnimIndex;
public:
	static void	 Add_CitizenWalkRunAnimIndex(const std::string& strFolderName , CModel* pPrototypeModel);
	static const CitizenWalkRunAnimIndex& Get_CitizenWalkRunAnimIndex(const std::string& strFolderName);
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
	std::array<CITIZEN_PART_DATA, ENUM_TO_UINT(DTO::CITIZEN_PARTTYPE::END)>	  arrayPartDatas;

	_bool isUseClothColorMapping;
	SHADER_RGBCOLOR_DESC tClothRGBColor;

	std::string				strModelName;
	std::string				strLoopAnimationName; /* 단순 Citizen NPC들이 계속 Loop할 애니매이션 목록 */


public:
	/* Preset으로 만들어진 애들은 모두 Run & Anim Index를 가지고 태어난다 전용 */
	CitizenWalkRunAnimIndex tWalkRunAnimIndex;
public:
	CITIZEN_DATA()
		: arrayNpcAtlasData{ CITIZEN_ATLAS_DATA(true , 4 , 4 , 0 , 0) , CITIZEN_ATLAS_DATA(true , 3 , 3 ,0, 0) }
		, isUseClothColorMapping{ true }
		, tClothRGBColor{}
		, strLoopAnimationName{}
		, strModelName{ "" }
		, tWalkRunAnimIndex{}
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
		, tWalkRunAnimIndex{rhs.tWalkRunAnimIndex }
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
		tWalkRunAnimIndex = rhs.tWalkRunAnimIndex;
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

inline const std::wstring& wstrCitizenWaypointDatasPath{L"../../Resources/Data/CitizenData/CitizenWayPointData.json"};
struct ENGINE_DLL Citizen_WayPoint_Data
{
	vector<Vec3>	vecPosition{};					/* 움직일 Position 모음 */
	Vec3			vStartPitchYawRoll{0.f,0.f,0.f};	/* WayPoint 시작할떄 NPC가 회전해 있을 Rotation값 */
	float			fDuration{};
private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	/* Debug Line을 위한 용도 */
	PrimitiveBatch<DirectX::VertexPositionColor>* pBatch;
	BasicEffect* pEffect;
	ID3D11InputLayout* pInputLayout;
public:
	Citizen_WayPoint_Data(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
		: pDevice(_pDevice), pContext{_pContext}
		, vecPosition{}
		, vStartPitchYawRoll{}
		, pEffect{nullptr}
		, pBatch{nullptr}
		, pInputLayout{nullptr}
		, fDuration{20.f}
	{
		Safe_AddRef(pDevice);
		Safe_AddRef(pContext);

		pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);
		pEffect = new BasicEffect(pDevice);

		const void* pShaderInput = { nullptr };
		size_t iShaderInputLenght = {};
		pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
		pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &pInputLayout);

	}
	Citizen_WayPoint_Data(const Citizen_WayPoint_Data& rhs)
		: pDevice(rhs.pDevice), pContext{ rhs.pContext }
		, vecPosition{rhs.vecPosition}
		, vStartPitchYawRoll{rhs.vStartPitchYawRoll}
		, pEffect{ nullptr }
		, pBatch{ nullptr }
		, pInputLayout{ nullptr }
		, fDuration{rhs.fDuration }
	{
		Safe_AddRef(pDevice);
		Safe_AddRef(pContext);


		pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);
		pEffect = new BasicEffect(pDevice);

		const void* pShaderInput = { nullptr };
		size_t iShaderInputLenght = {};
		pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
		pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &pInputLayout);

	}
	Citizen_WayPoint_Data& operator=(const Citizen_WayPoint_Data& rhs)
	{
		this->vecPosition	= rhs.vecPosition;
		this->vStartPitchYawRoll = rhs.vStartPitchYawRoll;


		const void* pShaderInput = { nullptr };
		size_t iShaderInputLenght = {};
		this->pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
		this->pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &this->pInputLayout);
		this->fDuration = rhs.fDuration;


		return *this;
	}
	virtual ~Citizen_WayPoint_Data()
	{
		Safe_Release(pDevice);
		Safe_Release(pContext);

		Safe_Delete(pBatch);
		Safe_Delete(pEffect);
		Safe_Release(pInputLayout);
	}

public:
	void			Render_Debug(CShader* pShader, CModel* pModel, _uint iPassIndex);
public:
	void			Load_Json(const json& LoadJson);
	void			Save_Json(json& LoadJson);
}; 
struct ENGINE_DLL CitizenWayPointOriginData
{
	static inline std::map<std::string, vector<Citizen_WayPoint_Data>> mapCitizenWapointDatas{};

	static HRESULT	Load_CitizenWayPointDatas(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext);
	static HRESULT	Load_CitizenWayPointDatas(const string& strLevelName, _uint iIndex, Citizen_WayPoint_Data& tOutData);

	static HRESULT	Save_CitizenWayPointDatas(const string& strLevelName, const Citizen_WayPoint_Data& tData, _int iIndex = -1);
	static HRESULT	Save_CitizenWayPointDatas();

	static void		Render_CitizenWayPointRenderDebug(const string& strLevelName, _uint iIndex, CShader* pShader, CModel* pModel, _uint iPassIndex);
	static const Citizen_WayPoint_Data* Get_RandomWayPointOrignData(const string& strLevelName);
public:
	static void Clear()
	{
		mapCitizenWapointDatas.clear();
	}
};




/* Preset Data 같은 느낌? */
/* Pool에 등록을 해도되고... */



inline const std::wstring& wstrCitizenPresetDatasPath{ L"../../Resources/Data/CitizenData/CitizenPresetData.json" };
struct ENGINE_DLL CitizenPresetData
{
public:
	static inline _uint iCurIndexForRandom{0};
	static inline std::vector<CITIZEN_DATA>	vecDatas{};
	// 파일 입출력
public:
	static HRESULT				Add_ModelPrototype(_uint iAddPrototypeLevel ,ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	static CITIZEN_DATA			Get_Preset(_uint iIndex);	
	static CITIZEN_DATA&		Get_Preset_ForTool(_uint iIndex);
	static CITIZEN_DATA			Get_Preset_ForRandom();
public:
	static HRESULT				Load_CitizenPresetData();
	static HRESULT				Save_CitizenPresetData(); //JSON은 통째로 굽는 게 안전하므로 인덱스 제거

	// 메모리 제어 (추가/수정/삭제)
	static HRESULT				Update_CitizenPresetData(const CITIZEN_DATA& tData, _int iIndex = -1);
	static HRESULT				Delete_CitizenPresetData(_int iIndex);
public:
	static void Clear()
	{
		vecDatas.clear();
	}
};



enum class CITIZEN_MOVE_TYPE
{
	WALK,	// 걷기
	RUN,	// 뛰기
	END,
};

/* Random Anim  */
struct Citizen_MoveData
{
	const Citizen_WayPoint_Data*	pWayPointData{ nullptr };
	CITIZEN_MOVE_TYPE				eMoveType{ CITIZEN_MOVE_TYPE::WALK }; // 이동 타입 (기본값 세팅)
	float							fSpeed{ 2.5 };                       // 타입에 맞는 이동 속도
};

static Citizen_MoveData Get_RandomCitizenMoveData(const std::string& strLevelName)
{
	/* 그리고 Way Point 랜덤으로 뽑아내기 */

	Citizen_MoveData tData{};

	// 예시: 50% 확률로 걷거나 뜀 (rand() 사용 예제)
	int iRandom = CGameInstance::GetInstance()->Rand_Int(0, 1);

	if (iRandom == 0)
	{
		tData.eMoveType = CITIZEN_MOVE_TYPE::WALK;
		tData.fSpeed = 1.0f; // 걷기 속도
	}
	else
	{
		tData.eMoveType = CITIZEN_MOVE_TYPE::RUN;
		tData.fSpeed = 4.0f; // 뛰기 속도
	}

	tData.pWayPointData = CitizenWayPointOriginData::Get_RandomWayPointOrignData(strLevelName);

	return tData;
}


static void AllCitizenDatas_Clear()
{
	DTO::CitizenWayPointOriginData::Clear();
	DTO::CitizenPresetData::Clear();
}


/* Random 이름 Data */

struct CitizenUITextData
{
public:

	static constexpr size_t AGE_MAX = static_cast<size_t>(DTO::CITIZEN_TYPE::END);
	static constexpr size_t GENDER_MAX = static_cast<size_t>(DTO::CITIZEN_GENDERTYPE::END);

	using NameArray2D	= std::array<std::array<std::vector<std::string>, GENDER_MAX>, AGE_MAX>;

	using TextArray2D	= std::array<std::array<std::vector<std::string>, GENDER_MAX>, AGE_MAX>;

	using OffsetArray	= std::array< Vec3 , AGE_MAX>;

private:
	/* Random 이름값 */
	static inline NameArray2D arrNameDatas = { {
			// [0] Child (CITIZEN_TYPE::Child)
			{{
				{"민준", "서준", "도윤", "시우", "하준", "지훈"}, // [0] Male
				{"서연", "하은", "지아", "서윤", "지유", "다은"}  // [1] Female
			}},
		// [1] Youth (CITIZEN_TYPE::Youth)
		{{
			{"현우", "민수", "준호", "지민", "성민", "동현"},
			{"민지", "지은", "수아", "유진", "혜진", "소민"}
		}},
		// [2] Middle (CITIZEN_TYPE::Middle)
		{{
			{"정훈", "성호", "상철", "기태", "영수", "동수"},
			{"미경", "선영", "정희", "영미", "현주", "은주"}
		}},
		// [3] Old (CITIZEN_TYPE::Old)
		{{
			{"덕배", "춘배", "만수", "칠성", "용식", "팔봉"},
			{"옥자", "춘자", "점순", "말숙", "순자", "끝순"}
		}}
	} };

	static inline TextArray2D arrTextDatas = { {
			// [0] Child (골목 누비기, 뛰어다님, 시선 분산)
			{{
				{"쥬신 학원에 늦겟어 빨리 가야겠어.", "빨리 가서 자리 맡아야지!", "우와, 이 골목 엄청 좁다."}, // Male
				{"고양이 어디로 숨었지?", "엄마가 빨리 오랬는데...", "이쪽 길로 가면 빠르려나?"}   // Female
			}},
		// [1] Youth (발걸음 재촉, 환경 불평, 혼잣말)
		{{
			{"아, 먼지 진짜 많네.", "오늘따라 순찰대가 자주 보이네.", "빨리 배달하고 쉬어야지."}, // Male
			{"이 골목은 맨날 물이 고여있어.", "아휴, 해 지기 전에 가야 하는데.", "지름길로 가면 금방 가겠지."}  // Female
		}},
		// [2] Middle (짐 나르기, 바쁘게 걷기, 생활 밀착형 불평)
		{{
			{"어허, 길 좀 비켜주쇼! 지나갑니다.", "오늘따라 짐이 더 무겁네.", "바람이 차네. 비 오려나."}, // Male
			{"어유, 골목에 쓰레기 좀 그만 버리지.", "오늘 저녁거리가 영 시원찮네.", "바쁘다 바빠, 빨리 가자."}  // Female
		}},
		// [3] Old (천천히 걷기, 주변 관찰, 느릿한 혼잣말)
		{{
			{"에잉, 바닥이 미끄러우니 조심해야지.", "이 동네도 참 많이 변했어.", "천천히 가야지. 서두를 게 뭐 있나."}, // Male
			{"아이고, 오늘 따라 허리가 더아픈겨..", "어디 보자... 이쪽 길이 맞던가.", "바람이 제법 쌀쌀하구먼."}  // Female
		}}
	} };


	static inline OffsetArray arrTextOffsets = { {
			{ 0.f, 2.f, 0.f }, // [0] Child
			{ 0.f, 3.f, 0.f }, // [1] Youth
			{ 0.f, 3.f, 0.f }, // [2] Middle
			{ 0.f, 3.f, 0.f }  // [3] Old
		} };

public:
	static inline std::string Get_RandomCitizenName(const std::wstring& wstrFolderName)
	{
		CitizenModelInfo tInfo = Get_CitizenInfo_ByClothPath(wstrFolderName);

		size_t iAge = static_cast<size_t>(tInfo.eAge);
		size_t iGender = static_cast<size_t>(tInfo.eGender);

		if (iAge >= AGE_MAX || iGender >= GENDER_MAX)
			return std::string("이름모를 시민");

		const std::vector<std::string>& vecNames = arrNameDatas[iAge][iGender];

		if (vecNames.empty())
			return std::string("이름모를 시민");

		_uint iMaxIndex = static_cast<_uint>(vecNames.size() - 1);
		return vecNames[CGameInstance::GetInstance()->Rand_Int(0, iMaxIndex)];
	}

	static inline std::string Get_RandomCitizenText(const std::wstring& wstrFolderName)
	{
		CitizenModelInfo tInfo = Get_CitizenInfo_ByClothPath(wstrFolderName);

		size_t iAge = static_cast<size_t>(tInfo.eAge);
		size_t iGender = static_cast<size_t>(tInfo.eGender);

		if (iAge >= AGE_MAX || iGender >= GENDER_MAX)
			return std::string("");

		const std::vector<std::string>& vecTexts = arrTextDatas[iAge][iGender];

		if (vecTexts.empty())
			return std::string("");

		_uint iMaxIndex = static_cast<_uint>(vecTexts.size() - 1);
		return vecTexts[CGameInstance::GetInstance()->Rand_Int(0, iMaxIndex)];
	}

	static inline Vec3 Get_CitizenTextOffset(const std::wstring& wstrFolderName)
	{
		CitizenModelInfo tInfo = Get_CitizenInfo_ByClothPath(wstrFolderName);
		size_t iAge = static_cast<size_t>(tInfo.eAge);

		if (iAge >= AGE_MAX)
			return Vec3(0.f, 3.f, 0.f);

		return arrTextOffsets[iAge];
	}
};



NS_END



