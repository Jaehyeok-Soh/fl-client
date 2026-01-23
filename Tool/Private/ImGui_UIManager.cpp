#include "ImGui_UIManager.h"
#include "Tool_Defines.h"

IMPLEMENT_SINGLETON(CImGui_UIManager)

CImGui_UIManager::CImGui_UIManager()
{
}

const CANVAS_DATA& CImGui_UIManager::Get_CurCanvasData() const
{
	if (m_vecCanvasData.empty())
	{
		MSG_BOX("CImGui_UIManager::Get_CurCanvasDataRef, Empty Vector");
		return m_EmptyData;
	}
	return m_vecCanvasData[m_iCurCanvasIndex];
}

CANVAS_DATA& CImGui_UIManager::Get_CurCanvasDataRef()
{
	if (m_vecCanvasData.empty())
	{
		MSG_BOX("CImGui_UIManager::Get_CurCanvasDataRef, Empty Vector");
		return m_EmptyData; 
	}
	return m_vecCanvasData[m_iCurCanvasIndex];
}

_bool CImGui_UIManager::HasCanvasTag(const _string& tag)
{
	if (tag == "")
		return FALSE;

	/* 현재 Editor에 있는 CanvasTag를 순회 */
	for (const auto& Desc : m_vecCanvasData)
	{
		/* 만약 CurTag가 vector에 있다면 */
		if (Desc.strTag == tag)
			return TRUE;
	}

	return FALSE;
}

uint32_t CImGui_UIManager::CanvasTagToIndex(const _string& tag)
{
	uint32_t index = { 0 };
	for (const auto& Desc : m_vecCanvasData)
	{
		/* 만약 Tag가 vector에 있다면 */
		if (Desc.strTag == tag)
			return index;
		++index;
	}
	/* 없으면 현재 인덱스를 반환 */
	return m_iCurCanvasIndex;
}

void CImGui_UIManager::Free()
{
	Super::Free();
}

