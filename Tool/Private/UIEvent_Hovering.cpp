#include "pch.h"
#include "Tool_Defines.h"

#include "UIEvent_Hovering.h"

CUIEvent_Hovering::CUIEvent_Hovering(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:IUIEvent(pDevice, pContext)
{
}

CUIEvent_Hovering::CUIEvent_Hovering(const CUIEvent_Hovering& rhs)
	:IUIEvent(rhs)
{
}

HRESULT CUIEvent_Hovering::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIEvent_Hovering::Initialize(void* pArg)
{
	UIEVENT_HOVERING_DESC* pDesc = static_cast<UIEVENT_HOVERING_DESC*>(pArg);

	Super::Initialize(pArg);
	return S_OK;
}

CUIEvent_Hovering* CUIEvent_Hovering::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIEvent_Hovering* pInstance = new CUIEvent_Hovering(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIEvent_Hovering Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

IUIEvent* CUIEvent_Hovering::Clone(void* pArg)
{
	CUIEvent_Hovering* pInstance = new CUIEvent_Hovering(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIEvent_Hovering Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIEvent_Hovering::Free()
{
	Super::Free();
}
