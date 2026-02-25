#pragma once
#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "json_forward.h"
#include "Engine_Utils.h"



NS_BEGIN(Engine)
struct ENGINE_DLL CLIENT_MAKEPATH_DESC_BASE
{
public:
	explicit CLIENT_MAKEPATH_DESC_BASE()
	{

	}
	/* 작성 권장 */
	explicit CLIENT_MAKEPATH_DESC_BASE(const CLIENT_MAKEPATH_DESC_BASE& rhs)
	{
		return;
	}
	virtual ~CLIENT_MAKEPATH_DESC_BASE() {};
public:
	virtual void	from_Json(const json& LoadJson)PURE;
	virtual void	to_Json(json& SaveJson)PURE;
};

#pragma region 파싱용 Description 작성

#pragma region Static Object

struct ENGINE_DLL STATICOBJECT_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	using Super = CLIENT_MAKEPATH_DESC_BASE;
	wstring			wstrTest{ L"" };
public:
	explicit STATICOBJECT_DESC()
		: wstrTest{ L"" }
	{
	}
	explicit STATICOBJECT_DESC(const STATICOBJECT_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), wstrTest(rhs.wstrTest)
	{
		return;
	}
	virtual ~STATICOBJECT_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};


#pragma endregion

#pragma region LandScape

struct ENGINE_DLL LANDSCAPE_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	using Super = CLIENT_MAKEPATH_DESC_BASE;

	_int		iIndex{0};
	/* 지형 Texture UV 좌표  */
	Vec2		vTextureUV_LT{};
	Vec2		vTextureUV_RB{};


	/* Texture */
	ID3D11ShaderResourceView* m_pTexture{};

public:
	explicit LANDSCAPE_DESC()
		: vTextureUV_LT{ Vec2::Zero }, vTextureUV_RB{Vec2::Zero}
	{
	}
	explicit LANDSCAPE_DESC(const LANDSCAPE_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), vTextureUV_RB(rhs.vTextureUV_RB) , vTextureUV_LT(rhs.vTextureUV_LT) , iIndex(rhs.iIndex)
	{
		return;
	}
	virtual ~LANDSCAPE_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion



#pragma region Trigger Box

struct ENGINE_DLL TRIGGERBOX_DESC : CLIENT_MAKEPATH_DESC_BASE
{
public:
	Vec3		 vExtents{0.5f,0.5f ,0.5f};
public:
	explicit TRIGGERBOX_DESC()
		: vExtents{ 0.5f,0.5f ,0.5f }
	{
	}
	explicit TRIGGERBOX_DESC(const TRIGGERBOX_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs) , vExtents(rhs.vExtents)
	{
		return;
	}
	virtual ~TRIGGERBOX_DESC() {}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};


#pragma region ChangeLevel

struct ENGINE_DLL TRIGGERBOX_CHANGELEVEL_DESC : TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	std::string strChangeLevelTypeName{ "NONE" };
public:
	explicit TRIGGERBOX_CHANGELEVEL_DESC()
		: TRIGGERBOX_DESC(), strChangeLevelTypeName{"NONE"}
	{
	}
	explicit TRIGGERBOX_CHANGELEVEL_DESC(const TRIGGERBOX_CHANGELEVEL_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), strChangeLevelTypeName{rhs.strChangeLevelTypeName}
	{
		return;
	}
	virtual ~TRIGGERBOX_CHANGELEVEL_DESC() {};
public:
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion


#pragma region Spawner


#pragma endregion

#pragma endregion


#pragma endregion
NS_END

NS_BEGIN(DTO)
/////////////////-------------------  MAP  -------------------/////////////////

enum class EMapObject_Type : _uint
{
	MAPOBJECT,
	SCENEDATA,
	END
};

enum class EClientLevelType : unsigned int
{
	STATIC = 0,
	LOADING,
	LOGO,
	END
};

enum class EMapObject_DrawType
{
	Collider,
	Default,
	Instance,
	END
};

enum class EClientMakePath
{
	StaticObject,
	LandScape,
	Bush,
	Grass,
	Moss,
	Tree,
	Vine,
	Rock,
	Water,



	/* 단순 객체 */
	Monster_Dog,
	Monster_Shooter,

	/* Trigger Box 관련 */
	TriggerBox_ChangeLevel,
	TriggerBox_MonsterSpawner,


	END
};


inline constexpr _uint g_MapObjectTypeCount{ ENUM_TO_UINT(EMapObject_Type::END) };
inline constexpr _uint g_MapObjecTDrawTypeCount{ ENUM_TO_UINT(EMapObject_DrawType::END) };
inline constexpr _uint g_ClientLevelType{ ENUM_TO_UINT(EClientLevelType::END) };
inline constexpr _uint g_ClientMakePath{ ENUM_TO_UINT(EClientMakePath::END)};

/* String으로 자동 변환되어서 저당된다  */

NLOHMANN_JSON_SERIALIZE_ENUM(EMapObject_Type,
	{
		{EMapObject_Type::MAPOBJECT, "MAPOBJECT"},
		{EMapObject_Type::END, "UnKnown"},
	}
	)

NLOHMANN_JSON_SERIALIZE_ENUM(EClientLevelType,
		{
			{EClientLevelType::LOADING, "LOADING"},
			{EClientLevelType::LOGO,	"LOGO"},
			{EClientLevelType::STATIC,	"STATIC"},
			{EClientLevelType::END,		"UnKnown"},
		}
		)
NLOHMANN_JSON_SERIALIZE_ENUM(EMapObject_DrawType,
		{
			{EMapObject_DrawType::Collider, "Collider"},
			{EMapObject_DrawType::Default,	"Default"},
			{EMapObject_DrawType::Instance,	"Instance"},
			{EMapObject_DrawType::END,		"END"},
		}
		)

	NLOHMANN_JSON_SERIALIZE_ENUM(EClientMakePath,
		{
			{EClientMakePath::StaticObject,					"StaticObject"},
			{EClientMakePath::LandScape,					"LandScape"},
			{EClientMakePath::Bush,							"Bush"},
			{EClientMakePath::Grass,						"Grass"},
			{EClientMakePath::Moss,							"Moss"},
			{EClientMakePath::Tree,							"Tree"},
			{EClientMakePath::Vine,							"Vine"},
			{EClientMakePath::Rock,							"Rock"},
			{EClientMakePath::Water,						"Water"},



			{EClientMakePath::Monster_Dog,					"Monster_Dog"},
			{EClientMakePath::Monster_Shooter,				"Monster_Shooter"},


			{EClientMakePath::TriggerBox_ChangeLevel,		"TriggerBox_ChangeLevel"},
			{EClientMakePath::TriggerBox_MonsterSpawner,	"TriggerBox_MonsterSpawner"},

			{EClientMakePath::END,							"Unknown"},
		}
		)

	/////////////////-------------------  ObjectStruct  -------------------/////////////////

#pragma region Map Data 구조체


#pragma region SRT Data
	typedef struct tagSRT_Data
{
	Vec3	vScale{0.f,0.f,0.f};
	Quat	vQuat{0.f,0.f,0.f,1.f};
	Vec3	vPosition{ 0.f,0.f,0.f };
	Vec3	vScale_Isolated{}; //TEST: 소재혁 임시 추가
public:
	Matrix  Get_World()
	{
		return Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
	}
}SRT_DATA;
#pragma endregion

#pragma region Using Material 

typedef struct tagOverrideMaterials
{
	bool	isNull{ false };

	/* Override 할때 참조하고 있는 파일 Path 값 */
	wstring wstrMtl_JsonFile_Name{};
	wstring wstrMtl_JsonFile_Path{};
	/* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
	vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
}OVERRIDE_MATERIALS;
#pragma endregion
#pragma region Using Model
typedef struct tagUsingModelInfo
{
	wstring wstrName{};
	wstring wstrPath{};

	/* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
	wstring wstrMtl_JsonFile_Path{};

	vector<OVERRIDE_MATERIALS> vecOverrideMaterial{};
public:

}USING_MODEL_INFO;
#pragma endregion

#pragma region MapObject

typedef struct TMap_MapObjectData
{
	/* UE Load Check */
	_uint								iSectionNum{};
	bool								isUELoaded{ false };
	string								strUERawDataPath{""};
	string								strTag{};
	string								strModelPath{ "" };

	/* Client Make Level Type */
	_uint								eClientMakePath{};
	_uint								eClientLevelType{};
	_uint								eMapObjectDrawType{};

	/* SRT Data , Cient Make Path Desc */
	vector<SRT_DATA>					vecSRTs{};
	vector<CLIENT_MAKEPATH_DESC_BASE*>	vecClientMakePathDesc{};
}MAPOBJECTDATA;
#pragma endregion



#pragma endregion


#pragma region Level Data

typedef struct TLevelData
{
	string								strTag{};
	string								strTextureSplatingInfoName{"None"};
	string								strLevelTypeName{"STATIC"};

}SCENEDATA;

#pragma endregion

/////////////////-------------------  to_json, from_json  -------------------/////////////////
#pragma region SRT Data
inline void to_json(json& SaveJson, const SRT_DATA& tData);
inline void from_json(const json& LoadJson, SRT_DATA& tdata);
#pragma endregion 

#pragma region Using Material 

void from_json(const json& LoadJson, OVERRIDE_MATERIALS& tData);

void to_json(json& SaveJson, const OVERRIDE_MATERIALS& tData);

#pragma endregion

#pragma region Using Model
void from_json(const json& LoadJson, USING_MODEL_INFO& tData);
void to_json(json& SaveJson, const USING_MODEL_INFO& tData);

#pragma endregion

#pragma region Map Object
inline void to_json(json& SaveJson, const TMap_MapObjectData& tData);
inline void from_json(const json& LoadJson, TMap_MapObjectData& tData);
#pragma endregion


#pragma region Level Data

inline void to_json(json& SaveJson, const TLevelData& tData);
inline void from_json(const json& LoadJson, TLevelData& tData);

#pragma endregion

NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////
NS_BEGIN(Engine)

#pragma region Mapobject

class CData_MapObject final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
	using Super = IObjectDataBase;
private:
	CData_MapObject() = default;
	virtual ~CData_MapObject() = default;
public:

public:
	_uint							Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT);}
	const string&					Get_Tag() const override { return m_tData.strTag; }
	
	json							ToJson() const override;
	HRESULT							FromJson(const json& j) override;

	const DTO::TMap_MapObjectData&	Get_Data() const { return m_tData; }
	DTO::TMap_MapObjectData&		Get_Data() { return m_tData; }
private:
	DTO::TMap_MapObjectData			m_tData{};
public:
	static CData_MapObject* Create() { return new CData_MapObject(); }
	virtual void Free() override;
};
#pragma endregion

#pragma region Level Data

class CData_LevelData final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
	using Super = IObjectDataBase;
private:
	CData_LevelData() = default;
	virtual ~CData_LevelData() = default;
public:

public:
	_uint							Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA); }
	const string&					Get_Tag() const override { return m_tData.strTag; }

	json							ToJson() const override;
	HRESULT							FromJson(const json& j) override;

	const DTO::TLevelData&			Get_Data() const { return m_tData; }
	DTO::TLevelData&				Get_Data() { return m_tData; }
private:
	DTO::TLevelData					m_tData{};
public:
	static CData_LevelData* Create()
	{
		return new CData_LevelData();
	}
	virtual void Free() override;
};
#pragma endregion

NS_END




NS_BEGIN(Engine)

ENGINE_DLL CLIENT_MAKEPATH_DESC_BASE* Create_ClientMakePathDesc(DTO::EClientMakePath ePath, CLIENT_MAKEPATH_DESC_BASE* pSource =nullptr);
ENGINE_DLL _bool					  IsExist_ClientMakePathDesc(DTO::EClientMakePath ePath);

NS_END


