#include "pch.h"
#include "ImGuiBase.h"
#include "GameInstance.h"

CImGuiBase::CImGuiBase(std::string_view label)
	: m_strLabel(label)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CImGuiBase::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}