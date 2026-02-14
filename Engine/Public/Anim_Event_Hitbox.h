#pragma once
#include "Anim_Event_Script_Base.h"
#include "Physics_QueryFilterCallback.h"

#include "PhysX\PxPhysics.h"
#include "PhysX\PxPhysicsAPI.h"
using namespace physx;

#include "Engine_Typedef.h"

#include "Engine_Enum.h"

namespace DTO
{
	typedef struct tagHitboxDesc : public ANIM_EVENT_SCRIPT_BASE
	{

		PxGeometryHolder geometry;
		PxQueryFilterData filterData = { PxQueryFilterData() };
		Matrix matOffset;
		CPhysics_QueryFilterCallback* filterCallback = { nullptr };

		OverlapType::Enum eType = OverlapType::Enum::SPHERE;
		Vec3 vExtents = {};
		_float fRadius = {};
		Vec3 vOffset = {};

		_float fTickTime = { -1.f };

		_float fDamage = {};

		_uint iMaxHit = { 32 };

		PHYSICSFILTERGROUP::Enum eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
		_uint iFilterMask = {};

		////////////////////////////
		// ANIM_EVENT_SCRIPT_BASE //
		////////////////////////////
		//_string strName = {};
		//_float fDuration = {};
		//float Get_Duration() { return fDuration; }
	}HITBOX_DESC;

	inline void to_json(json& j, const HITBOX_DESC& d)
	{
		j["eType"] = d.eType;
		j["vExtents"] = { { "x", d.vExtents.x },{ "y", d.vExtents.y },{ "z", d.vExtents.z } };
		j["fRadius"] = d.fRadius;
		j["vOffset"] = { { "x", d.vOffset.x },{ "y", d.vOffset.y },{ "z", d.vOffset.z } };
		j["fTickTime"] = d.fTickTime;
		j["fDamage"] = d.fDamage;
		j["iMaxHit"] = d.iMaxHit;
		j["eFilterLayer"] = d.eFilterLayer;
		j["iFilterMask"] = d.iFilterMask;
	}

	inline void from_json(const json& j, HITBOX_DESC& d)
	{
		j.at("eType").get_to(d.eType);
		
		const auto& jext = j.at("vExtents");
		jext.at("x").get_to(d.vExtents.x);
		jext.at("y").get_to(d.vExtents.y);
		jext.at("z").get_to(d.vExtents.z);
		
		j.at("fRadius").get_to(d.fRadius);

		const auto& joff = j.at("vOffset");
		joff.at("x").get_to(d.vOffset.x);
		joff.at("y").get_to(d.vOffset.y);
		joff.at("z").get_to(d.vOffset.z);

		j.at("fTickTime").get_to(d.fTickTime);
		j.at("fDamage").get_to(d.fDamage);
		j.at("iMaxHit").get_to(d.iMaxHit);
		j.at("eFilterLayer").get_to(d.eFilterLayer);
		j.at("iFilterMask").get_to(d.iFilterMask);
	}
}