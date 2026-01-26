#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
#pragma pop_macro("new")

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

typedef struct tagCanvasData CANVAS_DATA;
typedef struct tagLayerData LAYER_DATA;
typedef struct tagGenericUIData GENERIC_UI_DATA;

class CGenericUI;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Load_UIData(const _wstring& wstrSaveFilePath);

private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free()override;

};

typedef struct tagCanvasData
{
	_string strTag;
	_bool isUsingViewport;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;

	vector<LAYER_DATA> vecLayers;
}CANVAS_DATA;

typedef struct tagLayerData
{
	_string strTag;

	vector<GENERIC_UI_DATA> vecUIData;
}LAYER_DATA;

typedef struct tagGenericUIData
{
	uint32_t iUIType;
	uint32_t iRectTransformType;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;
}GENERIC_UI_DATA;

void from_json(const json& _j, CANVAS_DATA& _tData);
void from_json(const json& _j, LAYER_DATA& _tData);
void from_json(const json& _j, GENERIC_UI_DATA& _tData);

NS_END