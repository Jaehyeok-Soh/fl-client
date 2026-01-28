#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;
#pragma pop_macro("new")

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Tool)


typedef struct tagCanvasData CANVAS_DATA;
typedef struct tagLayerData LAYER_DATA;
typedef struct tagGenericUIData GENERIC_UI_DATA;

typedef struct tagUIDTO UI_DTO;
typedef struct tagLayerDTO LAYER_DTO;
typedef struct tagCanvasDTO CANVAS_DTO;

class CToolUI;

enum class EUiType {
	IMAGE_2D = 0, IMAGE_3D, BUTTON, TEXT, VIDEO, END
};

enum class ERectTransform {
	LEFTTOP = 0, TOP, RIGHTTOP, LEFT, CENTER, RIGHT, LEFTBOTTOM, BOTTOM, RIGHTBOTTOM, END
};

class CUIData_Repository final : public CBase
{
	using Super = CBase;

	DECLARE_SINGLETON(CUIData_Repository)

private:
	CUIData_Repository();
	virtual ~CUIData_Repository() = default;

public:
	HRESULT Load_UIData(OUT vector<CANVAS_DATA>& OutRef);
	HRESULT Save_UIData();

private:
	vector<CANVAS_DATA> m_vecUIDTO;
	CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free()override;
};

static const _string& UITypeToString(EUiType eType)
{
	switch (eType)
	{
	case Tool::EUiType::IMAGE_2D:
		return "IMAGE_2D";
	case Tool::EUiType::IMAGE_3D:
		return "IMAGE_3D";
	case Tool::EUiType::BUTTON:
		return "BUTTON";
	case Tool::EUiType::TEXT:
		return "TEXT";
	case Tool::EUiType::VIDEO:
		return "VIDEO";
	}
	return "";
}
static EUiType StringToUIType(const _string& str)
{
	if (::strcmp(str.c_str(), "IMAGE_2D") == 0)
		return EUiType::IMAGE_2D;
	else if (::strcmp(str.c_str(), "IMAGE_3D") == 0)
		return EUiType::IMAGE_3D;
	else if (::strcmp(str.c_str(), "BUTTON") == 0)
		return EUiType::BUTTON;
	else if (::strcmp(str.c_str(), "TEXT") == 0)
		return EUiType::TEXT;
	else if (::strcmp(str.c_str(), "VIDEO") == 0)
		return EUiType::VIDEO;
	else
		return EUiType::END;
}
static const _string& RectTransformToString(ERectTransform eType)
{
	static const _string sLEFTTOP = "LEFTTOP";
	static const _string sTOP = "TOP";
	static const _string sRIGHTTOP = "RIGHTTOP";
	static const _string sLEFT = "LEFT";
	static const _string sCENTER = "CENTER";
	static const _string sRIGHT = "RIGHT";
	static const _string sLEFTBOTTOM = "LEFTBOTTOM";
	static const _string sBOTTOM = "BOTTOM";
	static const _string sRIGHTBOTTOM = "RIGHTBOTTOM";
	static const _string sEMPTY = "";

	switch (eType)
	{
	case Tool::ERectTransform::LEFTTOP:
		return sLEFTTOP;
	case Tool::ERectTransform::TOP:
		return sTOP;
	case Tool::ERectTransform::RIGHTTOP:
		return sRIGHTTOP;
	case Tool::ERectTransform::LEFT:
		return sLEFT;
	case Tool::ERectTransform::CENTER:
		return sCENTER;
	case Tool::ERectTransform::RIGHT:
		return sRIGHT;
	case Tool::ERectTransform::LEFTBOTTOM:
		return sLEFTBOTTOM;
	case Tool::ERectTransform::BOTTOM:
		return sBOTTOM;
	case Tool::ERectTransform::RIGHTBOTTOM:
		return sRIGHTBOTTOM;
	}
	return sEMPTY;
}
static ERectTransform StringToRectTransform(const _string& str)
{
	if (::strcmp(str.c_str(), "LEFTTOP") == 0)
		return ERectTransform::LEFTTOP;
	else if (::strcmp(str.c_str(), "TOP") == 0)
		return ERectTransform::TOP;
	else if (::strcmp(str.c_str(), "RIGHTTOP") == 0)
		return ERectTransform::RIGHTTOP;
	else if (::strcmp(str.c_str(), "LEFT") == 0)
		return ERectTransform::LEFT;
	else if (::strcmp(str.c_str(), "CENTER") == 0)
		return ERectTransform::CENTER;
	else if (::strcmp(str.c_str(), "RIGHT") == 0)
		return ERectTransform::RIGHT;
	else if (::strcmp(str.c_str(), "LEFTBOTTOM") == 0)
		return ERectTransform::LEFTBOTTOM;
	else if (::strcmp(str.c_str(), "BOTTOM") == 0)
		return ERectTransform::BOTTOM;
	else if (::strcmp(str.c_str(), "RIGHTBOTTOM") == 0)
		return ERectTransform::RIGHTBOTTOM;
	else
		return ERectTransform::END;
}

typedef struct tagGenericUIData
{
	/* For.Runtime */
	_string strName;

	uint32_t iUIType;
	uint32_t iRectTransformType;
	_string strTextureTag;
	uint32_t iTextureIndex;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;

	void Clear_Data()
	{
		strName = "";
		iUIType = 0;
		iRectTransformType = 4;
		strTextureTag = "";
		iTextureIndex = 0;
		fWidth = 0.f;
		fHeight = 0.f;
		fPosX = 0.f;
		fPosY = 0.f;
		fPosZ = 0.f;
	}

}GENERIC_UI_DATA;

typedef struct tagLayerData
{
	_string strTag;

	void Clear_Data()
	{
		strTag = "";
	}
}LAYER_DATA;

typedef struct tagCanvasData
{
	_string strTag;
	_bool isUsingViewport;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;

	void Clear_Data()
	{
		strTag = "";
		isUsingViewport = FALSE;
		fWidth = 0.f;
		fHeight = 0.f;
		fPosX = 0.f;
		fPosY = 0.f;
		fPosZ = 0.f;
	}
}CANVAS_DATA;


typedef struct tagUIDTO
{
	GENERIC_UI_DATA UIVO;

}UI_DTO;

typedef struct tagLayerDTO
{
	LAYER_DATA LayerVO;

	vector<UI_DTO> vecUIDTO;

}LAYER_DTO;

typedef struct tagCanvasDTO
{
	CANVAS_DATA CanvasVO;

	vector<LAYER_DTO> vecLayerDTO;

}CANVAS_DTO;

void to_json(order_json& _j, const CANVAS_DATA& _tData);
void from_json(const order_json& _j, CANVAS_DATA& _tData);
void to_json(order_json& _j, const LAYER_DATA& _tData);
void from_json(const order_json& _j, LAYER_DATA& _tData);
void to_json(order_json& _j, const GENERIC_UI_DATA& _tData);
void from_json(const order_json& _j, GENERIC_UI_DATA& _tData);

void to_json(order_json& _j, const UI_DTO& _tData);
void from_json(const order_json& _j, UI_DTO& _tData);
void to_json(order_json& _j, const LAYER_DTO& _tData);
void from_json(const order_json& _j, LAYER_DTO& _tData);
void to_json(order_json& _j, const CANVAS_DTO& _tData);
void from_json(const order_json& _j, CANVAS_DTO& _tData);

NS_END