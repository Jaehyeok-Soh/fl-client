#include "pch.h"
#include "Button.h"
#include "GameInstance.h"

CButton::CButton()
{
}

CButton::CButton(const CButton& rhs)
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);
	return S_OK;
}

void CButton::Update(const _float fTimeDelta)
{

}

void CButton::OnClick()
{
}

CButton* CButton::Create(const BUTTON_DESC& Desc)
{
	CButton* pInstance = new CButton();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CButton::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CButton::Clone(void* pArg)
{
	CButton* pInstance = new CButton(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CButton::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CButton::Free()
{
	Super::Free();
}


