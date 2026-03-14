#include "Engine_pch.h"
#include "Anim_Event_Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include <string>
#include "Quest_DataModel.h"
#include "Engine_Enum.h"

void DTO::to_json(json& j, const QUESTDESC& d)
{
	j["iId"] = d.iId;
	j["iPrevId"] = d.iPrevId;
	j["iNextId"] = d.iNextId;
	j["iParentId"] = d.iParentId;
	j["wstrTitle"] = Engine_Utils::ToString(d.wstrTitle);
	j["wstrSubTitle"] = Engine_Utils::ToString(d.wstrSubTitle);
	j["wstrExplain"] = Engine_Utils::ToString(d.wstrExplain);
	j["wstrDescription"] = Engine_Utils::ToString(d.wstrDescription);
}

void DTO::from_json(const json& j, QUESTDESC& d)
{
	j.at("iId").get_to(d.iId);
	j.at("iPrevId").get_to(d.iPrevId);
	j.at("iNextId").get_to(d.iNextId);
	j.at("iParentId").get_to(d.iParentId);

	string strBuffer = {};
	j.at("wstrTitle").get_to(strBuffer);
	d.wstrTitle = Engine_Utils::ToWString(strBuffer);
	
	j.at("wstrSubTitle").get_to(strBuffer);
	d.wstrSubTitle = Engine_Utils::ToWString(strBuffer);
	
	j.at("wstrExplain").get_to(strBuffer);
	d.wstrExplain = Engine_Utils::ToWString(strBuffer);
	
	j.at("wstrDescription").get_to(strBuffer);
	d.wstrDescription = Engine_Utils::ToWString(strBuffer);
}

void DTO::to_json(json& j, const QUEST_CHAPTERDESC& d)
{
	j["eEvent"] = d.eEvent;
	j["eTargetType"] = d.eTargetType;
	j["iCount"] = d.iCount;
	j["tQuestDesc"] = d.tQuestDesc;
}

void DTO::from_json(const json& j, QUEST_CHAPTERDESC& d)
{
	j.at("eEvent").get_to(d.eEvent);
	j.at("eTargetType").get_to(d.eTargetType);
	j.at("iCount").get_to(d.iCount);
	j.at("tQuestDesc").get_to(d.tQuestDesc);
}