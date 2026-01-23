#pragma once
#include "Base.h"

NS_BEGIN(Tool)

class CToolObject;

class CPicking_ToolManager final : public CBase
{
	using SUPER = CBase;
	DECLARE_SINGLETON(CPicking_ToolManager)
private:
	CPicking_ToolManager();
	virtual ~CPicking_ToolManager() = default;
public:
	_bool Picking();
	void Add_PickingGroup(CToolObject* pGo);
	void Remove_PickingGroup(CToolObject* pGo);
	void Clear_Picking();
	void Destroy_PickingManager();
public:
	const Vec3& Get_PickingPos() const { return m_vPickingPos; }
	_bool Picking_ForDummy();
private:
	Vec3 m_vPickingPos = {};
	std::unordered_set<CToolObject*> m_PickingList;
};

NS_END