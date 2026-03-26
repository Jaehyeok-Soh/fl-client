#include "pch.h"
#include "State_HitFlyStart.h"

#include "Player.h"

CState_HitFlyStart::CState_HitFlyStart(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "HitFlyStart")
{
}

HRESULT CState_HitFlyStart::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitFlyStart::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitFlyStart::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	HITSTATE_START_DESC* pDesc = static_cast<HITSTATE_START_DESC*>(pArg);

	CTransform* pOwnerTransform = Get_OwnerObject()->Get_Component<CTransform>();
	if (pOwnerTransform)
	{
		Vec3 vMyPos		= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vTargetPos = vMyPos + (pDesc->vHitDir * -1.f);

		pOwnerTransform->Look_At_XZ(vTargetPos);
	}

	Look_Impuls(-10.f);
	Jump_Impuls(1.f);

	return S_OK;
}

void CState_HitFlyStart::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// 바닥 충돌 검사 후 change
	if (m_fStateElapsed > 0.28f &&
		(Check_OnGround(0.3f) || IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_SIDES))
		)
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::HITFLYEND));
		return;
	}
}

HRESULT CState_HitFlyStart::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

_bool CState_HitFlyStart::Can_Captablity_Move() const
{
	return false;
}

CState_HitFlyStart* CState_HitFlyStart::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_HitFlyStart* pInstance = new CState_HitFlyStart(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_HitFlyStart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_HitFlyStart::Free()
{
	Super::Free();
}
