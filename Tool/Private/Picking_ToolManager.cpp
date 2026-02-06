#include "pch.h"
#include "Picking_ToolManager.h"
#include "ToolObject.h"
#include "ImGui_ToolManager.h"
#include "GameObject.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPicking_ToolManager)

CPicking_ToolManager::CPicking_ToolManager()
{
    m_PickingList.reserve(50);
}

_bool CPicking_ToolManager::Picking()
{
	//if (!MOUSE_LBUTTON_DOWN || ImGuizmo::IsUsing() || ImGuizmo::IsOver()) return false;
	//if (ImGuizmo::IsOver())
	//	return false;
	if (ImGuizmo::IsUsing())
		return false;
	if (!MOUSE_LBUTTON_DOWN)
		return false;


	if (CImGui_ToolManager::GetInstance()->IsOutofViewport())
		return false;

	vector<CToolObject*> vecPicked;
	vector<Vec3> vecPos;

	for (auto it = m_PickingList.begin(); it != m_PickingList.end(); )
	{
		CToolObject* obj = *it;
		if (!obj) { it = m_PickingList.erase(it); continue; }

		Vec3 hitW;
		if (obj->Picking(hitW)) {
			vecPicked.push_back(obj);
			vecPos.push_back(hitW);
		}
		++it;
	}
	if (vecPicked.empty())
	{
		CGameInstance::GetInstance()->Broadcast<ChangeSelectedObject>(nullptr);
		return false;
	}
	const Matrix& matView = CGameInstance::GetInstance()->Get_ViewMatrix();

	int best = -1;
	float bestZ = FLT_MAX;
	for (int i = 0; i < (int)vecPos.size(); ++i)
	{
		Vec3 vEye = Vec3::Transform(vecPos[i], matView);
		if (vEye.z > 0.f && vEye.z < bestZ) { bestZ = vEye.z; best = i; }
	}
	if (best < 0)
	{
		CGameInstance::GetInstance()->Broadcast<ChangeSelectedObject>(nullptr);
		return false;
	}

	m_vPickingPos = vecPos[best];
	if(CGameInstance::GetInstance()->KeyButton_Pressing(DIK_LSHIFT) == false)
		CGameInstance::GetInstance()->Broadcast<ChangeSelectedObject>(vecPicked[best]);

	return true;

}

void CPicking_ToolManager::Add_PickingGroup(CToolObject* pGo)
{
    if (!pGo)
        return;

    Safe_AddRef(pGo);
    m_PickingList.insert(pGo);
}

void CPicking_ToolManager::Remove_PickingGroup(CToolObject* pGo)
{
    unordered_set<CToolObject*>::iterator itr = m_PickingList.find(pGo);
    if (itr != m_PickingList.end())
    {
        Safe_Release(pGo);
        m_PickingList.erase(itr);
    }
}

void CPicking_ToolManager::Clear_Picking()
{
    for (CToolObject* pGo : m_PickingList)
    {
        Safe_Release(pGo);
    }
    m_PickingList.clear();
}

void CPicking_ToolManager::Destroy_PickingManager()
{
	Clear_Picking();
	CPicking_ToolManager::GetInstance()->DestroyInstance();
}

_bool CPicking_ToolManager::Picking_ForDummy()
{
	if (ImGuizmo::IsUsing() || ImGuizmo::IsOver()) return FALSE;

	if (CImGui_ToolManager::GetInstance()->IsOutofViewport()) return FALSE;

	vector<CToolObject*> vecPicked;
	vector<Vec3> vecPos;

	for (auto it = m_PickingList.begin(); it != m_PickingList.end(); )
	{
		CToolObject* obj = *it;
		if (!obj) { it = m_PickingList.erase(it); continue; }

		Vec3 hitW;
		if (obj->Picking(hitW)) {
			vecPicked.push_back(obj);
			vecPos.push_back(hitW);
		}
		++it;
	}
	if (vecPicked.empty())
		return false;

	const Matrix& matView = CGameInstance::GetInstance()->Get_ViewMatrix();

	int best = -1;
	float bestZ = FLT_MAX;
	for (int i = 0; i < (int)vecPos.size(); ++i)
	{
		Vec3 vEye = Vec3::Transform(vecPos[i], matView);
		if (vEye.z > 0.f && vEye.z < bestZ) { bestZ = vEye.z; best = i; }
	}
	if (best < 0)
		return false;

	m_vPickingPos = vecPos[best];
	return FALSE;
}
