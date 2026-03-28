#include "pch.h"
#include "Lianhuo_GimmikController.h"
#include "Boss_Lianhuo.h"
#include "Boss_Lianhuo_Body.h"
#include "GameInstance.h"

CLianhuo_GimmikController::CLianhuo_GimmikController()
	: Super()
{
}

CLianhuo_GimmikController::CLianhuo_GimmikController(const CLianhuo_GimmikController& rhs)
	: Super(rhs)
{
}

HRESULT CLianhuo_GimmikController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_GimmikController::Bind_Events()
{
	return S_OK;
}

void CLianhuo_GimmikController::On_ModelAnimNotify(const AnimNotifyKey& key)
{

}

CLianhuo_GimmikController* CLianhuo_GimmikController::Create()
{
	CLianhuo_GimmikController* pInstance = new CLianhuo_GimmikController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CLianhuo_GimmikController::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CLianhuo_GimmikController::Clone(void* pArg)
{
	CLianhuo_GimmikController* pInstance = new CLianhuo_GimmikController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLianhuo_GimmikController::Create(), Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLianhuo_GimmikController::Free()
{
	Super::Free();
}