#include "pch.h"
#include"Tool_Defines.h"
#include "UIButton_Component.h"


CUIButton_Component::CUIButton_Component()
{
}

CUIButton_Component::CUIButton_Component(const CMonoBehaviour& rhs)
{
}

HRESULT CUIButton_Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton_Component::Awake(_uint iLevelIndex)
{
	return S_OK;
}

HRESULT CUIButton_Component::Initialize(void* pArg)
{
	return S_OK;
}

void CUIButton_Component::Update(const _float fTimeDelta)
{
}

CUIButton_Component* CUIButton_Component::Create()
{
	CUIButton_Component* pInstance = new CUIButton_Component();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIButton_Component::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CUIButton_Component::Clone(void* pArg)
{
	CUIButton_Component* pInstance = new CUIButton_Component(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIButton_Component::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIButton_Component::Free()
{
	Super::Free();
}