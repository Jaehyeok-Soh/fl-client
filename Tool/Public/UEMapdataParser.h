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

class CMapToolManager;

typedef struct tagParsedMapdataOuter	PARSED_MAPDATA_OUTER;
typedef struct tagUnreal_Map_Data		UE_MAP_DATA;
typedef struct tagCONVERTED_MAPDATA		CONVERTED_MAPDATA;
typedef struct tagUsingModelInfo		USING_MODEL_INFO;
typedef struct tagOverrideMaterials		OVERRIDE_MATERIALS;


class CUEMapdataParser final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CUEMapdataParser)
public:
	enum class EObject_Path_Type
	{
		Model,
		Mtl,
		Tex,
		END,
	};
	enum class EFilter_Type
	{
		StaticMesh,
		InstanceStaticMesh,
		END,
	};
private:
	CUEMapdataParser();
	virtual ~CUEMapdataParser() = default;
	bool	Filter(const string& strName,const string& strType);
	vector<MAPDATA_BASE*>		Convert_UE_MapData(const vector<UE_MAP_DATA>& tData);
	void						Change_SRT(OUT SRT_DATA& tSRT_Data);
	void						Change_ObjectPath(OUT _wstring& wstrModelName , OUT _wstring& wstrModelPath , EObject_Path_Type eType);
	void						Change_UsingModelInfo(OUT USING_MODEL_INFO& tUsingModelInfo);
	void						Change_Material_JsonFile_Path(OUT wstring& wstrMaterialJsonFilePath,const wstring& wstrModelPath);
	void						Change_OverrideMaterials(OUT OVERRIDE_MATERIALS& tUsingMtlInfo);
	void						Change_UsingMaterialTexturePath(OUT OVERRIDE_MATERIALS& tUsingMtlInfo);
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	_bool						Get_IsUseCheckAndBindInstance() const { return m_isUseCheckAndBindInstance; }
	vector<UE_MAP_DATA>*		Get_Unreal_MapData(const wstring& FindKey);
	vector<MAPDATA_BASE*>*		Get_Converted_MapData(const wstring& FindKey);


	void						Set_IsUseCheckAndBindInstance(_bool isUse) { m_isUseCheckAndBindInstance = isUse; }
	void						Set_MulScale(float fMulScale) { m_fMulScale = fMulScale; }
	float						Get_MulScale() const { return m_fMulScale; }

	vector<wstring>				Get_ConvertedFilePathList();

public:
	HRESULT						Convert_UnrealRawMapData(const wchar_t* wszUERawDataJsonFile);
	HRESULT						Batch_UnrealRawMapData(const wchar_t* wwszFilePath);

public:
	HRESULT						Save_ConvertedRawMapData(const wchar_t* wszFilePath);
	HRESULT						Save_FilteringRawMapData(const wchar_t* wszFilePath);

public:
	bool					m_isUseCheckAndBindInstance{false};
	const wstring			m_WstringConverted{ L"_Converted.json" };
	const wstring			m_WstringFiltering{ L"_Filtering.json" };
	Vec3					m_vMulPitchYawRoll{0.f,0.f,0.f};
private:
	
	/* Pass 별로 파싱된 데이터들이 담길텐데 */
	unordered_map< wstring, vector<MAPDATA_BASE*>>		m_umapConvertedMapData{};

	unordered_map< wstring , vector<UE_MAP_DATA>>		m_umapUnreal_Map_Data{};
	vector<string>										m_vecTypeFilter{};
	vector<string>										m_vecOuterFilter{};
	float												m_fMulScale{0.01f};


	const Matrix										m_ChangeMatrix	  =
	{
		1,  0,  0,  0,
		0,  0, -1,  0,
		0,  1,  0,  0,
		0,  0,  0,  1
	};


private:
	vector<string>										m_vecMtlTextureFilter{};

private:
	CMapToolManager*	m_pMapToolManager{nullptr};
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
	bool					m_isFiltering{ false };

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
	bool	m_isFiltering{false};

	string strType{};
	string strName{};
	string strOuter{};

	UE_PROPERTIES					tProperties{};
	vector<UE_PER_INSTANCE_SM_DATA>	vecPerInstanceSMData{};
public:
	EMapObject_Type Type_ToEnum()
	{
		if (strType.find("Instance") != string::npos)				return EMapObject_Type::INSTANCEMODEL;
		else if (strType.find("StaticMesh") != string::npos )		return EMapObject_Type::STATICMODEL;

		return EMapObject_Type::END;
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


#pragma endregion


NS_END
