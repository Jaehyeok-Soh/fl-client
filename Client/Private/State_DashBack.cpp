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

	m_fCapHitMoveTime = Get_MoveBoneTime(50.f);

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

	Set_RootMotion_Apply(true);

	return S_OK;
}

void CState_DashBack::Update(const _float fTimeDelta)
{
	if (IsRemotePlayer())
	{
		Super::Update(fTimeDelta);
		return;
	}

	Super::Update(fTimeDelta);
}

HRESULT CState_DashBack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	//Set_RootMotion_Apply(false);

	return S_OK;
}

_uint CState_DashBack::Get_Capabilities() const
{
	if (m_fStateElapsed > 0.4f)
	{
		return	Super::Get_Capabilities();
	}

	else
	{
		_uint iFlag = Super::Get_Capabilities();
		return	ENUM_TO_UINT(Engine::StateCapability::SKILL) | iFlag;
	}
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
