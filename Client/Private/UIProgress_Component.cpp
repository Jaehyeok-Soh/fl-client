#include "pch.h"
#include "Client_Defines.h"
#include "UIProgress_Component.h"
#include "GenericUI.h"

CUIProgress_Component::CUIProgress_Component()
{
}

CUIProgress_Component::CUIProgress_Component(const CMonoBehaviour& rhs)
{
}

HRESULT CUIProgress_Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIProgress_Component::Initialize_Prototype(CGenericUI* pOwner)
{
	if (!pOwner)
		return E_FAIL;
	m_pOwner = pOwner;

	return S_OK;
}

HRESULT CUIProgress_Component::Awake(_uint iLevelIndex)
{
	return S_OK;
}

HRESULT CUIProgress_Component::Initialize(void* pArg)
{
	return S_OK;
}

void CUIProgress_Component::Update(const _float fTimeDelta)
{
}

CUIProgress_Component* CUIProgress_Component::Create(CGenericUI* pOwner)
{
	CUIProgress_Component* pInstance = new CUIProgress_Component();
	if (FAILED(pInstance->Initialize_Prototype(pOwner)))
	{
		MSG_BOX("CUIProgress_Component::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CUIProgress_Component::Clone(void* pArg)
{
	CUIProgress_Component* pInstance = new CUIProgress_Component(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIProgress_Component::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIProgress_Component::Free()
{
	Super::Free();
}