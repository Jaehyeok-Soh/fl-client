#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

//"Type": "StaticMeshActor"

NS_BEGIN(Tool)

typedef struct tagParsedMapdataOuter PARSED_MAPDATA_OUTER;

class CUEMapdataParser final : public CBase
{
	using Super = CBase;
public:
	typedef struct tagUEMapdataParserDesc
	{
		wstring wstrPath = L"";
	}MAPPARSER_DESC;
private:
	CUEMapdataParser();
	virtual ~CUEMapdataParser() = default;

	HRESULT Initialize(const MAPPARSER_DESC &desc);
public:
	HRESULT Read_Mapdata();
	HRESULT Write_Mapdata();
private:
	std::filesystem::path m_path;
	vector<PARSED_MAPDATA_OUTER> m_vecData;
public:
	static CUEMapdataParser* Create(const MAPPARSER_DESC &desc);
	virtual void Free() override;

	friend class CUEMapDataLoader;
};
typedef struct tagParsedMapdataInnerBrushBodySetup
{
	string strObjectName = {""};
	string strObjectPath = {""};
}PARSED_MAPDATA_INNER_BRUSHBODYSETUP;

typedef struct tagParsedMapdataInnerBrush
{
	string strObjectName = {""};
	string strObjectPath = {""};
}PARSED_MAPDATA_INNER_BRUSH;

typedef struct tagParsedMapdataStaticMesh
{
	string strObjectName = { "" };
	string strObjectPath = { "" };
}PARSED_MAPDATA_INNER_STATICMESH;

typedef struct tagParsedMapdataInner
{
	PARSED_MAPDATA_INNER_STATICMESH StaticMesh = {};
	Vec3 vPosition = { 0.f, 0.f, 0.f };
	Vec3 vPitchYawRoll = { 0.f, 0.f, 0.f };
	Vec3 vScale = { 0.f, 0.f, 0.f };
}PARSED_MAPDATA_INNER;

typedef struct tagParsedMapdataOuter
{
	string strType = { "" };
	string strName = { "" };
	PARSED_MAPDATA_INNER Properties = {};
}PARSED_MAPDATA_OUTER;

void to_json(json& _j, const PARSED_MAPDATA_INNER_STATICMESH& _tData);
void from_json(const json& _j, PARSED_MAPDATA_INNER_STATICMESH& _tData);

void to_json(json& _j, const PARSED_MAPDATA_INNER_BRUSH& _tData);
void from_json(const json& _j, PARSED_MAPDATA_INNER_BRUSH& _tData);

void to_json(json& _j, const PARSED_MAPDATA_INNER_BRUSHBODYSETUP& _tData);
void from_json(const json& _j, PARSED_MAPDATA_INNER_BRUSHBODYSETUP& _tData);

void to_json(json& _j, const PARSED_MAPDATA_OUTER& _tData);
void from_json(const json& _j, PARSED_MAPDATA_OUTER& _tData);

void to_json(json& _j, const PARSED_MAPDATA_INNER& _tData);
void from_json(const json& _j, PARSED_MAPDATA_INNER& _tData);

void read_vec3_defaultscale(const json& _j, Vec3& vOut);
void read_vec3_xyz(const json& _j, Vec3& vOut);
void read_vec3_PitchYawRoll(const json& _j, Vec3& vOut);
NS_END
