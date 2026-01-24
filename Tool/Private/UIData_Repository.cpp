#include "pch.h"
#include "UIData_Repository.h"

IMPLEMENT_SINGLETON(CUIData_Repository)

CUIData_Repository::CUIData_Repository()
{
}

void CUIData_Repository::Road_UIData()
{
}

void CUIData_Repository::Save_UIData()
{
}

void CUIData_Repository::Free()
{
	Super::Free();
}

void to_json(json& _j, const CANVAS_DATA& _tData)
{
	_j = json
	{
		{"Tag", _tData.strTag},
		{"UsingViewport", _tData.isUsingViewport},
		{"Width", _tData.iWidth},
		{"Height", _tData.iHeight},
		{"PosX", _tData.iPosX},
		{"PosY", _tData.iPosY},
		{"PosZ", _tData.iPosZ},
	};
}

void from_json(const json& _j, CANVAS_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
	_j.at("UsingViewport").get_to(_tData.isUsingViewport);
	_j.at("Width").get_to(_tData.iWidth);
	_j.at("Height").get_to(_tData.iHeight);
	_j.at("PosX").get_to(_tData.iPosX);
	_j.at("PosY").get_to(_tData.iPosY);
	_j.at("PosZ").get_to(_tData.iPosZ);
}
