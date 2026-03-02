#include "pch.h"
#include "State_HitStrong.h"

#include "GameObject.h"
#include "Transform.h"

CState_HitStrong::CState_HitStrong(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "HitStrong")
{
}

HRESULT CState_HitStrong::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitStrong::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitStrong::Start(void* pArg, _bool bForce)
{
	HITSTATE_START_DESC* pDesc = static_cast<HITSTATE_START_DESC*>(pArg);

	/* 충돌 방향을 통해 재생 애니메이션 인덱스 설정 */
	Vec3 vHitDir = pDesc->vHitDir;
	Vec3 vPlayerLook = Get_OwnerObject()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);

	_float	fDot	= vPlayerLook.Dot(vHitDir);
	Vec3	vCross	= vPlayerLook.Cross(vHitDir);

	if (fDot > 0.5f)
	{
		pDesc->iMainAnimIdx = ENUM_TO_UINT(HitStrong_AnimIdx::BACK);
	}

	else if (fDot < -0.5f)
	{
		pDesc->iMainAnimIdx = ENUM_TO_UINT(HitStrong_AnimIdx::FRONT);
	}

	else
	{
		if (vCross.y > 0)
		{
			pDesc->iMainAnimIdx = ENUM_TO_UINT(HitStrong_AnimIdx::LEFT);
		}
		else
			pDesc->iMainAnimIdx = ENUM_TO_UINT(HitStrong_AnimIdx::RIGHT);
	}

	if (FAILED(Super::Start(pDesc, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_HitStrong::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_HitStrong::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_HitStrong* CState_HitStrong::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_HitStrong* pInstance = new CState_HitStrong(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_HitStrong::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_HitStrong::Free()
{
	Super::Free();
}
