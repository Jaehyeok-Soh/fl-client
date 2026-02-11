#include "Engine_pch.h"
#include "UIAction_Registry.h"
NS_BEGIN(Engine)

CUIAction_Registry::CUIAction_Registry()
{
}

void CUIAction_Registry::Initialize_CommonAction()
{
	
}

void CUIAction_Registry::Initialize_CommonTargetAction()
{
	
}


CUIAction_Registry* CUIAction_Registry::Create()
{
	CUIAction_Registry* pInstance = new CUIAction_Registry();
	pInstance->Initialize_CommonAction();
	pInstance->Initialize_CommonTargetAction();
	return pInstance;
}

void CUIAction_Registry::Free()
{
	Super::Free();
}

NS_END