#pragma once
#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "Engine_Utils.h"

#pragma region 최초 사용법
/*
* - 각 Category당
*	 데이터 .h, .cpp (ex. DataStruct_Exmaple)
*    CDoucmentBase 상속 객체 (ex. DataDocument_Example)
*	 Tool, Client쪽 Builder 상속 객체 (ex. Builder_Example)
* - 아래 규칙에 따라 struct, to_json, from_json, wrapper class 선언
* - 만든 Struct헤더 DataDefine_Json.h헤더에 추가
*/
#pragma endregion

/*
* 1. 각 Struct는 Type을 가지며 강제 한다.
* 2. Struct 최상단에는 Type과 Tag가 선언 한다.
* 3. 구조체와 wrapper클래스를 1대1로 만들어야 한다.
*		데이터를 담는 struct, 그걸 wrapping하고 타입을 구분하기위한 class로 역할 분리
*		WarpperClass Engine_DLL 필수
*		cpp에도 구조체, enum class 에는 DTO 네임스페이스, 엔진선언부에는 Engine 네임스페이스
*		json이 헤더에 노출된다면?
*		헤더에는 전방선언용 "json_forward.h"선언, 헤더에서 json.hpp 인클루드 금지
*/
NS_BEGIN(DTO)

/////////////////-------------------  MAP  -------------------/////////////////

enum class EMapObject_Type : _uint
{
	STATICMODEL,
	INSTANCEMODEL,
	END
};

inline constexpr _uint g_MapObjectTypeCount{ ENUM_TO_UINT(EMapObject_Type::END) };

/* String으로 자동 변환되어서 저당된다  */

NLOHMANN_JSON_SERIALIZE_ENUM(EMapObject_Type,
	{
		{EMapObject_Type::STATICMODEL, "STATICMODEL"},
		{EMapObject_Type::INSTANCEMODEL, "INSTANCEMODEL"},
		{EMapObject_Type::END, "END"},
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
	bool	isNull{ true };

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

#pragma region Static Model

typedef struct TMap_StaticModelData 
{
	std::string strTag{ "Static Model" };
	static constexpr EMapObject_Type eType = EMapObject_Type::STATICMODEL;

	/* Model Info */
	USING_MODEL_INFO tUsingModelInfo{};
	SRT_DATA		 tSRTData{};
}STATICMODEL_DATA;
#pragma endregion
#pragma region Instance Model
typedef struct TMap_InstanceModelData 
{
	std::string strTag{ "Instance Model" };
	static constexpr EMapObject_Type eType = EMapObject_Type::INSTANCEMODEL;

	D3D11_USAGE		 eInstance_Usage{D3D11_USAGE_DEFAULT};

	USING_MODEL_INFO tUsingModelInfo{};
	vector<SRT_DATA> vecSRTData{};
}InstanceModel_Data;
#pragma endregion

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

#pragma region Static Model
inline void to_json(json& SaveJson, const TMap_StaticModelData& tData);
inline void from_json(const json& LoadJson, TMap_StaticModelData& tData);
#pragma endregion

#pragma region Instance Model
inline void to_json(json& SaveJson, const TMap_InstanceModelData& tData);
inline void from_json(const json& LoadJson, TMap_InstanceModelData& tData);
#pragma endregion
NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)



class CData_StaticModel final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
	using Super = IObjectDataBase;
private:
	CData_StaticModel() = default;
	virtual ~CData_StaticModel() = default;
public:

public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::STATICMODEL);}
	const string& Get_Tag() const override { return m_tData.strTag; }
	
	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::STATICMODEL_DATA& Get_Data() const { return m_tData; }
	DTO::STATICMODEL_DATA& Get_Data() { return m_tData; }
private:
	DTO::STATICMODEL_DATA		m_tData{};
public:
	static CData_StaticModel* Create() { return new CData_StaticModel(); }
	virtual void Free() override { Super::Free(); }
};

class CData_InstanceModel final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CData_InstanceModel() = default;
	virtual ~CData_InstanceModel() = default;
public:
	_uint			Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::INSTANCEMODEL); }
	const string&	Get_Tag() const override { return m_tData.strTag; }

	json			ToJson() const override;
	HRESULT			FromJson(const json& j) override;

	const DTO::InstanceModel_Data&	Get_Data() const { return m_tData; }
	DTO::InstanceModel_Data&		Get_Data() { return m_tData; }
private:
	DTO::InstanceModel_Data		m_tData{};
public:
	static CData_InstanceModel* Create() { return new CData_InstanceModel; }
	virtual void Free() override { Super::Free(); }
};


NS_END




