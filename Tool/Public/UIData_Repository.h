#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

using json = nlohmann::json;


NS_BEGIN(Tool)
typedef struct tagCanvasData CANVAS_DATA;

class CUIData_Repository final : public CBase
{
	using Super = CBase;

	DECLARE_SINGLETON(CUIData_Repository)

private:
	CUIData_Repository();
	virtual ~CUIData_Repository() = default;

public:
	void Road_UIData();
	void Save_UIData();

private:
	vector<CANVAS_DATA> m_vecCanvasData;

public:
	virtual void Free()override;
};

typedef struct tagCanvasData
{
	_string strTag;
	_bool isUsingViewport;

	int32_t iWidth;
	int32_t iHeight;
	int32_t iPosX;
	int32_t iPosY;
	int32_t iPosZ;
}CANVAS_DATA;

void to_json(json& _j, const CANVAS_DATA& _tData);
void from_json(const json& _j, CANVAS_DATA& _tData);

NS_END