#include "pch.h"
#include "State_DashBack.h"

#include "Player.h"
#include "Transform.h"

CState_DashBack::CState_DashBack(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "DashBack")
{
}

HRESULT CState_DashBack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashBack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashBack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;
	
	//CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	//_float moveps = pPlayerTrans->Get_MovePerSec(); // 속도

	//Vec3 vRight = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::RIGHT));
	//vRight.Normalize();

	//Vec3 accelation = vRight * moveps * -10.f; //  방향 * 속도

	//SetCCTImpuls(accelation);


	return S_OK;
}

void CState_DashBack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_DashBack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_DashBack* CState_DashBack::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_DashBack* pInstance = new CState_DashBack(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_DashBack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_DashBack::Free()
{
	Super::Free();
}
