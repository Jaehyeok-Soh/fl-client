#include "pch.h"
#include "Tool_Defines.h"
#include "UI_Action_Registry.h"

IMPLEMENT_SINGLETON(CUI_Action_Registry)

CUI_Action_Registry::CUI_Action_Registry()
{
}

void CUI_Action_Registry::Register_Action(const _string& strActionName, ActionFunc function)
{
}

void CUI_Action_Registry::Execute_Action(const _string& strActionName)
{
}

void CUI_Action_Registry::Clear_Actions()
{
}

void CUI_Action_Registry::Free()
{
	Super::Free();
}
