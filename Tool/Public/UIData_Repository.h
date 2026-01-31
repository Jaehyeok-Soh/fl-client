#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;
#pragma pop_macro("new")

#include "DataStruct_UI.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Tool)
class CToolUI;

enum class EUiType {
	IMAGE_2D = 0, IMAGE_3D, BUTTON, TEXT, VIDEO, END
};

enum class ERectTransform {
	LT = 0, CT, RT, LC, C, RC, LB, CB, RB, END
};

class CUIData_Repository final : public CBase
{
	using Super = CBase;

	DECLARE_SINGLETON(CUIData_Repository)

private:
	CUIData_Repository();
	virtual ~CUIData_Repository() = default;

public:
	HRESULT Load_UIData();
	HRESULT Save_UIData();

private:
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
	static const _string sLT = "LT";
	static const _string sCT = "CT";
	static const _string sRT = "RT";
	static const _string sLC = "LC";
	static const _string sC = "C";
	static const _string sRC = "RC";
	static const _string sLB = "LB";
	static const _string sCB = "CB";
	static const _string sRB = "RB";
	static const _string sEMPTY = "";

	switch (eType)
	{
	case ERectTransform::LT: return sLT;
	case ERectTransform::CT: return sCT;
	case ERectTransform::RT: return sRT;
	case ERectTransform::LC: return sLC;
	case ERectTransform::C:  return sC;
	case ERectTransform::RC: return sRC;
	case ERectTransform::LB: return sLB;
	case ERectTransform::CB: return sCB;
	case ERectTransform::RB: return sRB;
	default: break;
	}
	return sEMPTY;
}

static ERectTransform StringToRectTransform(const _string& str)
{
	if (::strcmp(str.c_str(), "LT") == 0) return ERectTransform::LT;
	if (::strcmp(str.c_str(), "CT") == 0) return ERectTransform::CT;
	if (::strcmp(str.c_str(), "RT") == 0) return ERectTransform::RT;
	if (::strcmp(str.c_str(), "LC") == 0) return ERectTransform::LC;
	if (::strcmp(str.c_str(), "C") == 0) return ERectTransform::C;
	if (::strcmp(str.c_str(), "RC") == 0) return ERectTransform::RC;
	if (::strcmp(str.c_str(), "LB") == 0) return ERectTransform::LB;
	if (::strcmp(str.c_str(), "CB") == 0) return ERectTransform::CB;
	if (::strcmp(str.c_str(), "RB") == 0) return ERectTransform::RB;
	return ERectTransform::END;
}
NS_END