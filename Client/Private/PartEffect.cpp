#include "pch.h"
#include "Client_Defines.h"
#include "PartEffect.h"

#include "EffectHandler.h"
#include "PhysicsRigidBody.h"

#include "Engine_Utils.h"
#include "GameInstance.h"

CPartEffect::CPartEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CPartEffect::CPartEffect(const CPartEffect& rhs)
	: Super(rhs)
	, m_eState(rhs.m_eState)
	, m_FEffFlags(rhs.m_FEffFlags)
	, m_bSpawnAlready(rhs.m_bSpawnAlready)
{
}

HRESULT CPartEffect::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartEffect::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PART_EFFECT_DESC* pDesc = static_cast<PART_EFFECT_DESC*>(pArg);

	if (FAILED(Ready_EffectHandler(pDesc)))
		return E_FAIL;

	if (CPhysicsRigidBody* pRigi = Get_Component<CPhysicsRigidBody>())
	{
		pRigi->Awake();
	}

	for (size_t i = 0; i < ENUM_TO_SZET(CPartEff_State::END); i++)
	{
		m_arrDuration_FloatsAcc[i] = 0.f;
		m_arrDelay_FloatsAcc[i] = 0.f;
	}

	m_arrDurationTime_Origin	= pDesc->arrState_DurationTimes;
	m_arrDelayTime_Origin		= pDesc->arrState_DelayTimes;
	m_FEffFlags					= pDesc->FPartEff_Flags;
	m_pMatSocket				= pDesc->pMatSocket;


	m_eState = CPartEff_State::IDLE;

	Get_Component<CEffectHandler>()->Setup_ForOwner(this);

	return S_OK;
}

HRESULT CPartEffect::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CPartEffect::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPartEffect::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Update_State(fTimeDelta);
}

void CPartEffect::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CPartEffect::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if(m_pMatSocket)
		Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));

	else
		Super::Update_CombinedWorldMatrix((*m_pMatParent));
}

void CPartEffect::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CPartEffect::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{

}

void CPartEffect::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CPartEffect::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CPartEffect::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

_bool CPartEffect::On_Hit(const HIT_DESC& hitDesc)
{
	return false;
}

HRESULT CPartEffect::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPartEffect::Change_State(CPartEff_State eNextState)
{
	if (m_eState == eNextState)
		return;

	End_State(m_eState);

	// 이전 state를 추적할 수 있도록 매게변수 값으로 넘겨줌
	Start_State(eNextState);

	m_eState = eNextState;
}

void CPartEffect::Start_State(CPartEff_State eState)
{
	size_t iIdx = ENUM_TO_SZET(eState);
	// acc time reset
	m_arrDuration_FloatsAcc[iIdx] = 0.f;
	m_arrDelay_FloatsAcc[iIdx] = 0.f;


	// 만약 로직 복잡해진다면 함수로 뺄것
	switch (eState)
	{
	case CPartEff_State::IDLE:
		break;

	case CPartEff_State::SPAWN:
		m_bSpawnAlready = false;
		break;

	case CPartEff_State::DESPAWN:
		int a = 0;
		break;
	}
}

void CPartEffect::Update_State(const _float fTimeDelta)
{
	switch (m_eState)
	{
	case CPartEff_State::IDLE:
		Idle_Update(fTimeDelta);
		break;

	case CPartEff_State::SPAWN:
		Spawn_Update(fTimeDelta);
		break;

	case CPartEff_State::DESPAWN:
		Despawn_Update(fTimeDelta);
		break;
	}
}

void CPartEffect::Idle_Update(const _float fTimeDelta)
{
	if (Engine_Utils::Has_Flag(m_FEffFlags, PartEff_Flag::Spawn_CoolTime))
	{
		size_t iIdx = ENUM_TO_SZET(CPartEff_State::IDLE);

		m_arrDuration_FloatsAcc[iIdx] += fTimeDelta;

		// 1. 상태 지속 시간 누적 : 만약 다 되었다면 spawn
		if (m_arrDuration_FloatsAcc[iIdx] >= m_arrDurationTime_Origin[iIdx])
		{
			Change_State(CPartEff_State::SPAWN);
			return;
		}
	}
}

void CPartEffect::Spawn_Update(const _float fTimeDelta)
{
	size_t iIdx = ENUM_TO_SZET(CPartEff_State::SPAWN);

	// 0. duration, 시간을 누적
	m_arrDuration_FloatsAcc[iIdx] += fTimeDelta;
	m_arrDelay_FloatsAcc[iIdx] += fTimeDelta;
	
	// 1. 상태 지속 시간 누적 : 만약 다 되었다면 despawn
	// todo : 만약 spawn 다음 state를 despawn을 하고 싶지 않다면..? 추가 적인 작업이 필요함
	if (m_arrDuration_FloatsAcc[iIdx] >= m_arrDurationTime_Origin[iIdx])
	{
		Change_State(CPartEff_State::DESPAWN);
		return;
	}

	// 2. 스폰이 아직 안 되었다면 -> delay 시간 추적해서 spawn
	if (!m_bSpawnAlready)
	{
		if (m_arrDelay_FloatsAcc[iIdx] >= m_arrDelayTime_Origin[iIdx])
		{
			Spawn_Effect();
		}
	}
}

void CPartEffect::Despawn_Update(const _float fTimeDelta)
{
	size_t iIdx = ENUM_TO_SZET(CPartEff_State::DESPAWN);

	// 0. duration, 시간을 누적
	m_arrDuration_FloatsAcc[iIdx] += fTimeDelta;
	m_arrDelay_FloatsAcc[iIdx] += fTimeDelta;

	// 1. 상태 지속 시간 누적 : 만약 다 되었다면 despawn
	// todo : 만약 spawn 다음 state를 despawn을 하고 싶지 않다면..? 추가 적인 작업이 필요함
	

	// despawn을 무조건 해야하기 때문에 먼저 체크를 한다
	if (m_arrDelay_FloatsAcc[iIdx] >= m_arrDelayTime_Origin[iIdx])
	{
 		Despawn_Effect();
	}


	if (m_arrDuration_FloatsAcc[iIdx] >= m_arrDurationTime_Origin[iIdx])
	{
		Change_State(CPartEff_State::IDLE);
		return;
	}
}

void CPartEffect::End_State(CPartEff_State eState)
{
	switch (eState)
	{
	case CPartEff_State::IDLE:
		break;

	case CPartEff_State::SPAWN:
		break;

	case CPartEff_State::DESPAWN:
		break;
	}
}

HRESULT CPartEffect::Spawn_Effect()
{
	if (FAILED(Get_Component<CEffectHandler>()->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN)))
		return E_FAIL;

	m_bSpawnAlready = true;

	return S_OK;
}

HRESULT CPartEffect::Despawn_Effect()
{
	return Get_Component<CEffectHandler>()->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_DESTROY);
}

HRESULT CPartEffect::Ready_EffectHandler(PART_EFFECT_DESC* pDesc)
{
	CEffectHandler::ANIM_EFFECT_HANDLER_DESC Desc{};

	for (auto& pData : pDesc->tEffectHandlerDesc)
	{
		CEffectHandler::STATE_VFX_DESC SkillDesc{};
		SkillDesc = pData.tSkillDesc;
		// part obj의 위치를 따라간다
		SkillDesc.pParentTransformMatrix = &m_matCombinedWorld;

		Desc.eType = pData.eHandlerType;
		Desc.mEffectState.emplace(pData.eEffState, SkillDesc);
	}

	if (FAILED(Add_Component<CEffectHandler>(/*Static*/ 0, L"Prototype_Component_EffectHandler_SkillObject", &Desc)))
		return E_FAIL;

	return S_OK;
}

CPartEffect* CPartEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPartEffect* pInstance = new CPartEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CPartEffect::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPartEffect::Clone(void* pArg)
{
	CPartEffect* pInstance = new CPartEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPartEffect::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPartEffect::Free()
{
	Super::Free();
}