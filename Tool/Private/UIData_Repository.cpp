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
		{"Width", _tData.fWidth},
		{"Height", _tData.fHeight},
		{"PosX", _tData.fPosX},
		{"PosY", _tData.fPosY},
		{"PosZ", _tData.fPosZ},
	};
}

void from_json(const json& _j, CANVAS_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
	_j.at("UsingViewport").get_to(_tData.isUsingViewport);
	_j.at("Width").get_to(_tData.fWidth);
	_j.at("Height").get_to(_tData.fHeight);
	_j.at("PosX").get_to(_tData.fPosX);
	_j.at("PosY").get_to(_tData.fPosY);
	_j.at("PosZ").get_to(_tData.fPosZ);
}
