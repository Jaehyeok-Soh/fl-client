#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")


NS_BEGIN(Engine)

class CMapFile_Manager : public CBase
{
	using Super = CBase;
public:
	CMapFile_Manager();
	virtual ~CMapFile_Manager();
public:
	HRESULT SaveData(const wstring& wstrSavePath, const MAPFILE_DATA& data);
	HRESULT LoadData(const wstring& wstrFilePath, OUT MAPFILE_DATA& outData);
public:
	static CMapFile_Manager* Create();
	virtual void Free() override;
};

void to_json(json& j, const TRANSFORM_SAVEDATA& data);
void from_json(const json& j, TRANSFORM_SAVEDATA& data);

void to_json(json& j, const TRIGGERBOX_SAVEDATA& data);
void from_json(const json& j, TRIGGERBOX_SAVEDATA& data);

void to_json(json& j, const MODEL_SAVEDATA& data);
void from_json(const json& j, MODEL_SAVEDATA& data);

void to_json(json& j, const COLLIDER_SAVEDATA& data);
void from_json(const json& j, COLLIDER_SAVEDATA& data);

void to_json(json& j, const CELL_SAVEDATA& data);
void from_json(const json& j, CELL_SAVEDATA& data);

void to_json(json& j, const LIGHT_SAVEDATA& data);
void from_json(const json& j, LIGHT_SAVEDATA& data);

void to_json(json& j, const POLYGON_SAVEDATA& data);
void from_json(const json& j, POLYGON_SAVEDATA& data);

void to_json(json& j, const MAPOBJECT_SAVEDATA& data);
void from_json(const json& j, MAPOBJECT_SAVEDATA& data);

void to_json(json& j, const EFFECT_PRESET_SNAPSHOT& data);
void from_json(const json& j, EFFECT_PRESET_SNAPSHOT& data);

void to_json(json& j, const MESHEFFECT_PREVIEW_SAVEDATA& data);
void from_json(const json& j, MESHEFFECT_PREVIEW_SAVEDATA& data);

void to_json(json& j, const MESHEEFFECT_PRESET_SAVEDATA& data);
void from_json(const json& j, MESHEEFFECT_PRESET_SAVEDATA& data);

void to_json(json& j, const MAPFILE_DATA& data);
void from_json(const json& j, MAPFILE_DATA& data);

NS_END