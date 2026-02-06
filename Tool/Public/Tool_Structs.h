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

		Vec3 vScale_Isolated{}; //TEST: 소재혁 임시 추가
	public:
		Matrix Get_World()
		{
			return Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
		}
	}SRT_DATA;

	typedef struct tagOverrideMaterials
	{
		bool	isNull{ false };
		/* 참조하고 있는 Origin Mrt Material Json 파일 Path 값 */
		wstring wstrMtl_JsonFile_Name{};
		wstring wstrMtl_JsonFile_Path{};
		/* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
		vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
	}OVERRIDE_MATERIALS;

	typedef struct tagUsingModelInfo
	{
		wstring wstrName{};
		wstring wstrPath{};

		/* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
		wstring wstrMtl_JsonFile_Path{};

		vector<OVERRIDE_MATERIALS> vecOverrideMaterial{};
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

		D3D11_USAGE		 eInstance_Usage{D3D11_USAGE_DEFAULT};

		vector<SRT_DATA> vecSRT{};
		vector<Matrix>	 vecMatirx{};
	public:
		virtual ~tagInstanceModel_Data() {}

	}INSTANCEMODEL_DATA;

	void to_json(json& SaveJson, const	 SRT_DATA& tData);
	void to_json(json& SaveJson, const	 OVERRIDE_MATERIALS& tData);
	void to_json(json& SaveJson, const	 USING_MODEL_INFO& tData);
	void to_json(json& SaveJson, const	 STATICMODEL_DATA& tData);

	void to_json(json& SaveJson, const	 STATICMODEL_DATA& tData);
	void to_json(json& SaveJson, const	 INSTANCEMODEL_DATA& tData);
	
	void from_json(const json& LoadJson, SRT_DATA& tData);
	void from_json(const json& LoadJson, OVERRIDE_MATERIALS& tData);
	void from_json(const json& LoadJson, USING_MODEL_INFO& tData);

	void from_json(const json& LoadJson, STATICMODEL_DATA& tData);
	void from_json(const json& LoadJson, INSTANCEMODEL_DATA& tData);

#pragma endregion 


}


