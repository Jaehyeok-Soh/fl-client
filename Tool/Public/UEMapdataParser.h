#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
#include <wincrypt.h>
using json = nlohmann::json;
#pragma pop_macro("new")

//"Type": "StaticMeshActor"

NS_BEGIN(Engine)

class CGameInstance;

NS_END


NS_BEGIN(Tool)

typedef struct tagParsedMapdataOuter PARSED_MAPDATA_OUTER;
typedef struct tagUnreal_Map_Data	 UE_MAP_DATA;
typedef struct tagCONVERTED_MAPDATA   CONVERTED_MAPDATA;

class CUEMapdataParser final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CUEMapdataParser)
private:
	CUEMapdataParser();
	virtual ~CUEMapdataParser() = default;
	bool	Filter(const string& strName,const string& strType);
	vector<CONVERTED_MAPDATA>  Convert_UE_MapData(const vector<UE_MAP_DATA>& tData);
	void					  Change_SRT(Vec3* vScale, Vec3* vPitchYawRoll, Vec3* vPosition, EStaticModel_Type eType);
	void					  Change_ModelPath(OUT _wstring& wstrModelName , OUT _wstring& wstrModelPath);
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Ready_PreUEMapRawDataLoad();
public:
	vector<UE_MAP_DATA>*	   Get_Unreal_MapData(const wstring& FindKey);
	vector<CONVERTED_MAPDATA>* Get_Converted_MapData(const wstring& FindKey);

	void					  Set_MulScale(float fMulScale) { m_fMulScale = fMulScale; }
	float					  Get_MulScale() const { return m_fMulScale; }

	vector<wstring>			  Get_ConvertedFilePathList();

public:
	HRESULT					  Convert_UnrealRawMapData(const wchar_t* wszUERawDataJsonFile);
	HRESULT					  Batch_UnrealRawMapData(const wchar_t* wwszFilePath);

public:
	HRESULT					  Save_ConvertedRawMapData(const wchar_t* wszFilePath);
	HRESULT					  Save_FilteringRawMapData(const wchar_t* wszFilePath);
private:
	
	unordered_map< wstring, vector<CONVERTED_MAPDATA>>  m_umapConvertedMapData{};
	unordered_map< wstring , vector<UE_MAP_DATA>>		m_umapUnreal_Map_Data{};
	vector<string>										m_vecTypeFilter{};
	float												m_fMulScale{0.01f};

	const wstring										m_WstringConverted{L"_Converted.json"};
	const wstring										m_WstringFiltering{L"_Filtering.json"};

private:
	CGameInstance*		 m_pGameInstance{nullptr};
	ID3D11Device*		 m_pDevice{nullptr};
	ID3D11DeviceContext* m_pContext{ nullptr };
public:
	virtual void Free() override;
	friend class CUEMapDataLoader;
};

#pragma region UE DATA

typedef struct tagUngreal_TransformData
{

	Vec3		vScale3D{1.f,1.f,1.f};
	Quat		vRotation{0.f,0.f,0.f,0.f};
	Vec3		vTranslation{0.f,0.f,0.f};

}UE_TRANSFORMDATA;
typedef struct tagUnreal_Per_Instance_SM_Data
{
	bool					isNull{ false };
	UE_TRANSFORMDATA		tTransformData{};
}UE_PER_INSTANCE_SM_DATA;
typedef struct tagUnreal_Object_Info
{
	string strObjectName{};
	string strObjectPath{};
}UE_OBJECT_INFO;
typedef struct tagUnreal_OverrideMaterials
{
	vector<bool>			isNull{false};
	vector<UE_OBJECT_INFO>	vecObjectInfo{};
}UE_OVERRIDEMATERIALS;
typedef struct tagUnreal_Properties
{
	_int32					iInstancingRandomSeed{ 0 };

	/* Rotatino = Pitch Yaw Roll */
	Vec3					vRelativeScale{1.f,1.f,1.f};
	Vec3					vRelativeRotation{0.f,0.f,0.f};
	Vec3					vRelativeLocation{0.f,0.f,0.f};
							
	UE_OBJECT_INFO			tStaticMesh{};
	UE_OBJECT_INFO			tAttachParent{};
	UE_OVERRIDEMATERIALS	tOverrideMaterials{};

}UE_PROPERTIES;
typedef struct tagUnreal_Map_Data
{
	string strType{};
	string strName{};
	string strOuter{};

	UE_PROPERTIES					tProperties{};
	vector<UE_PER_INSTANCE_SM_DATA>	vecPerInstanceSMData{};
public:
	EStaticModel_Type Type_ToString()
	{
		if (strType.find("Instance") != string::npos) return EStaticModel_Type::INSTANCE;
		else if (strType.find("StaticModel"))		  return EStaticModel_Type::DEFUALT;

		return EStaticModel_Type::END;
	}
}UE_MAP_DATA;

void to_json(json& SaveJson, const UE_OBJECT_INFO& tData);
void from_json(const json& LoadJson, UE_OBJECT_INFO& tData);

void to_json(json& SaveJson, const UE_OVERRIDEMATERIALS& tData);
void from_json(const json& LoadJson, UE_OVERRIDEMATERIALS& tData);

void to_json(json& SaveJson, const	UE_TRANSFORMDATA& tData);
void from_json(const json& LoadJson, UE_TRANSFORMDATA& tData);

void to_json(json& SaveJson, const	UE_PER_INSTANCE_SM_DATA& tData);
void from_json(const json& LoadJson, UE_PER_INSTANCE_SM_DATA& tData);

void to_json(json& SaveJson, const	 UE_PROPERTIES& tData);
void from_json(const json& LoadJson, UE_PROPERTIES& tData);

void to_json(json& SaveJson , const	  UE_MAP_DATA& tData);
void from_json(const json& LoadJson , UE_MAP_DATA& tData);


void read_vec3_defaultscale(const json& _j, Vec3& vOut);
void read_vec3_xyz(const json& _j, Vec3& vOut);
void read_vec3_PitchYawRoll(const json& _j, Vec3& vOut);
void read_vec4_Quat(const json& _j, Quat& vOut);

void write_vec3_xyz(json& _j, const Vec3& vOut);
void write_vec3_PitchYawRoll(json& _j, const Vec3& vOut);
void write_vec4_Quat(const json& _j, Quat&& vOut);


#pragma endregion


#pragma region Raw Data를 Convert 하고난 이후의 데이터를 담을 구조체

typedef struct tagUsingMaterialInfo
{
	bool	isNull{true};
	wstring wstrName{};
	wstring wstrPath{};
}USING_MATERIAL_INFO;


typedef struct tagUsingModelInfo
{
	wstring wstrName{};
	wstring wstrPath{};

	vector<USING_MATERIAL_INFO> vecMaterialInfo{};
}USING_MODEL_INFO;


typedef struct tagCONVERTED_MAPDATA
{
	/* Type */
	EStaticModel_Type eType{};
	/* Model Info */
	USING_MODEL_INFO tUsingModelInfo{};

	/* SRT */
	Vec3			 vPosition{ 0.f,0.f,0.f };
	Vec3			 vPitchYawRoll{0.f,0.f,0.f};
	Vec3			 vScale{1.f,1.f,1.f};
}CONVERTED_MAPDATA;


void to_json(json& SaveJson, const	 USING_MATERIAL_INFO& tData);
void to_json(json& SaveJson, const	 USING_MODEL_INFO& tData);
void to_json(json& SaveJson, const	 CONVERTED_MAPDATA& tData);


#pragma endregion


NS_END
