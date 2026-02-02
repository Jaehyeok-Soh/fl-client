#pragma once
#include "json_forward.h"

namespace Tool
{
#pragma region Map Data

	typedef struct tagSRTData
	{
		Vec3 vScale{};
		Quat vQuat{};
		Vec3 vPosition{};
	}SRT_DATA;

	typedef struct tagUsingMaterialInfo
	{
		bool	isNull{ true };
		/* 참조하고 있는 Origin Mrt Material Json 파일 Path 값 */
		wstring wstrOriginMtl_JsonFile_Name{};
		wstring wstrOriginMtl_JsonFile_Path{};
		/* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
		vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
	}USING_MATERIAL_INFO;

	typedef struct tagUsingModelInfo
	{
		wstring wstrName{};
		wstring wstrPath{};

		/* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
		wstring wstrMtl_JsonFile_Path{};

		vector<USING_MATERIAL_INFO> vecMaterialInfo{};
	public:

	}USING_MODEL_INFO;

	typedef struct tagMapData_Base
	{
		EMapObject_Type eMapObjectType{EMapObject_Type::END};
	public:
		virtual ~tagMapData_Base() {};
	}MAPDATA_BASE;

	/* Static Model Data */
	/* Loaded 된 애들이면 tSRT가 처음 Orgin Data로써 Reset 버튼 누를 시 작동한다 */
	typedef struct tagStaticModel_Data : public MAPDATA_BASE
	{
		USING_MODEL_INFO tUsingModelInfo{};
		/* Loaded Data */
		SRT_DATA		 tOriginSRT{};
	public:
		virtual ~tagStaticModel_Data() {}
	}STATICMODEL_DATA;

	/* Instance StaticModel Data */
	/* Loaded 된 애들이면 tSRT가 처음 Orgin Data로써 Reset 버튼 누를 시 작동한다 */
	typedef struct tagInstanceModel_Data : public MAPDATA_BASE
	{
		USING_MODEL_INFO tUsingModelInfo{};
		vector<SRT_DATA> vecOriginSRT{};
	public:
		virtual ~tagInstanceModel_Data() {}

	}INSTANCEMODEL_DATA;


	void to_json(json& SaveJson, const	 USING_MATERIAL_INFO& tData);
	void to_json(json& SaveJson, const	 USING_MODEL_INFO& tData);
	void to_json(json& SaveJson, const	 STATICMODEL_DATA& tData);



#pragma endregion 


}


