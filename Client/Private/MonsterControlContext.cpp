#include "pch.h"
#include "MonsterControlContext.h"

#include "Client_Defines.h"
#include "Monster_Base.h"
#include "MonsterActionState.h"

#include "GameInstance.h"
#include "Engine_Utils.h"

CMonsterControlContext::CMonsterControlContext()
	: Super()
{
}

CMonsterControlContext::CMonsterControlContext(const CMonsterControlContext& rhs)
	: Super(rhs)
{
}

HRESULT CMonsterControlContext::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterControlContext::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterControlContext::Awake(const _uint iLevelIndex)
{
	Set_Gravity(true);
	return S_OK;
}

_bool CMonsterControlContext::Is_FootRayEnabled()
{
	CGameObject* pOwner = Get_Owner();
	if (pOwner == nullptr)
		return true;

	CMonsterActionState* pActionState = pOwner->Get_Component<CMonsterActionState>();
	if (pActionState == nullptr)
		return true;

	const _int iIndex = pActionState->Get_CurrentStateIndex();
	if (iIndex <= -1)
		return true;

	CMonster_Base::State::Enum eState = static_cast<CMonster_Base::State::Enum>(iIndex);

	switch (eState)
	{
	case Client::CMonster_Base::State::IDLE:
	case Client::CMonster_Base::State::RUN:

		return true;
	default:
		return true;
	}
}

void CMonsterControlContext::Set_Grounded(_bool bGrounded, const COLMESH_HITINFO* pHit)
{
	Super::Set_Grounded(bGrounded);
	if (bGrounded && pHit)
		m_CurrentGroundInfo = *pHit;
}

void CMonsterControlContext::Clear_Grounded()
{
	Super::Set_Grounded(false);
	m_CurrentGroundInfo = {};
}

Vec3 CMonsterControlContext::Get_MoveDir()
{
	// TODO :
	// 타겟 - 몬스터
	// 몬스터 look

	return Vec3();
}

void CMonsterControlContext::OnChangeLockonTarget(CGameObject* pGo)
{
	if (pGo == m_pTarget)
		return;

	if (pGo)
		m_pTarget = pGo;
	else
		m_pTarget = nullptr;
}

CMonsterControlContext* CMonsterControlContext::Create()
{
	CMonsterControlContext* pInstance = new CMonsterControlContext();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonsterControlContext::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMonsterControlContext::Clone(void* pArg)
{
	CMonsterControlContext* pInstance = new CMonsterControlContext(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonsterControlContext::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterControlContext::Free()
{
	Super::Free();
}
