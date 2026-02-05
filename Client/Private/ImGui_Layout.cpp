#include "pch.h"
#include "ImGui_Layout.h"
#include "ImGui_Inspector.h"
#include "GameObject.h"
#include "GameInstance.h"

CImGui_Layout::CImGui_Layout(std::string_view label)
	: Super(label)
{
}

_bool CImGui_Layout::Can_Render(CGameObject* pGo)
{
	return (pGo != nullptr) && (pGo->IsDead() == false);
}

void CImGui_Layout::Free()
{
	Super::Free();
}