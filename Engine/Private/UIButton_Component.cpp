#include "Engine_pch.h"
#include "UIButton_Component.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

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
        return pInstance;
    }
    return pInstance;
}

CComponent* CUIButton_Component::Clone(void* pArg)
{
    CUIButton_Component* pInstance = new CUIButton_Component(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CUIButton_Component::Clone, Clone Failed");
        return pInstance;
    }
    return pInstance;
}

void CUIButton_Component::Free()
{
    Super::Free();
}

NS_END