#include "Engine_pch.h"
#include "SkillObject_Base.h"
#include "GameObject.h"
#include "PhysicsRigidBody.h"
#include "EffectHandler.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

CSkillObject_Base::CSkillObject_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CSkillObject_Base::CSkillObject_Base(const CSkillObject_Base& rhs)
	: Super(rhs)
	, m_iFlag(rhs.m_iFlag)
	, m_desc(rhs.m_desc)
	, m_runtimeDesc(rhs.m_runtimeDesc)
{

}

HRESULT CSkillObject_Base::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillObject_Base::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
		m_desc = *static_cast<SKILLOBJECT_DESC*>(pArg);

	m_iFlag = m_desc.iFlags;
	m_runtimeDesc.vStartPos = m_desc.vSpawnPos;
	m_runtimeDesc.vCurDir = m_desc.vDirection;
	m_runtimeDesc.Life.Start(m_desc.fLifeTime);
	if (m_runtimeDesc.vCurDir != Vec3::Zero)
		m_runtimeDesc.vCurDir.Normalize();

	if (Has_Flag(ESkillObjectFlag::Follow_Owner) && Has_Requester())
	{
		const Vec3 reqPos = m_desc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		// spawnPos를 월드로 받더라도 offset으로 변환
		m_runtimeDesc.vFollowOffset = m_desc.vSpawnPos - reqPos;
	}

	if (CEffectHandler* pEffectHandler = Get_Component<CEffectHandler>())
		pEffectHandler->Setup_ForOwner(this);

	return S_OK;
}

HRESULT CSkillObject_Base::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if(CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>())
		pRigidBody->Awake();

	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, m_desc.vSpawnPos);
	return S_OK;
}

void CSkillObject_Base::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CSkillObject_Base::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Process_Move(fTimeDelta);
	Process_Life(fTimeDelta);
}

void CSkillObject_Base::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CSkillObject_Base::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CSkillObject_Base::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSkillObject_Base::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (iOtherLayer == PHYSICSFILTERGROUP::Enum::MAP)
	{
		Set_Dead();
		return;
	}

	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyColliderLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	desc.tHitInfo = tHitInfo;
	m_pGameInstance->Push_CollidedData(desc);
}

HRESULT CSkillObject_Base::Spawn_FromPool(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	CTransform *pTransform = Get_Component<CTransform>();
	CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
	CEffectHandler* pEffectHandler = Get_Component<CEffectHandler>();

	SKILLOBJECT_DESC *pDesc = static_cast<SKILLOBJECT_DESC*>(pArg);
	m_desc = *pDesc;
	m_iFlag = m_desc.iFlags;
	m_runtimeDesc.vStartPos = m_desc.vSpawnPos;
	m_runtimeDesc.vCurDir = m_desc.vDirection;
	m_runtimeDesc.Life.Start(m_desc.fLifeTime);
	if (m_runtimeDesc.vCurDir != Vec3::Zero)
		m_runtimeDesc.vCurDir.Normalize();

	if (Has_Flag(ESkillObjectFlag::Follow_Owner) && Has_Requester())
	{
		const Vec3 reqPos = m_desc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		m_runtimeDesc.vFollowOffset = m_desc.vSpawnPos - reqPos;
	}
	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, m_desc.vSpawnPos);
	if (pRigidBody)
		pRigidBody->SetTransform(pTransform->Get_WorldMatrix());

	if(m_desc.eEffectRotateState >= TRANSFORM_INFO_STATE::POS)
		pEffectHandler->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN);
	else
		pEffectHandler->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN, m_desc.eEffectRotateState, m_desc.fEffectDegree);

	return S_OK;
}

HRESULT CSkillObject_Base::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_runtimeDesc = {};
	CEffectHandler* pHandler = Get_Component<CEffectHandler>();

	if (pHandler)
	{
		if (m_pGameInstance->Is_DestroyEngineSequence())
			pHandler->Clear_WhenChangeLevel();
		else
			pHandler->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_DESTROY);
	}
	return S_OK;
}

_bool CSkillObject_Base::Has_Flag(ESkillObjectFlag eFlag) const
{
	return Engine_Utils::Has_Flag(m_iFlag, ENUM_TO_UINT(eFlag));
}

void CSkillObject_Base::Process_Move(const _float fTimeDelta)
{
	CTransform* pTransform = Get_Component<CTransform>();
	if (Has_Flag(ESkillObjectFlag::Follow_Owner) && Has_Requester())
	{
		const Vec3 reqPos = m_desc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, reqPos + m_runtimeDesc.vFollowOffset);
		return;
	}

	const Vec3 vMoveDir = Compute_MoveDir(fTimeDelta, m_runtimeDesc.vCurDir);
	if (vMoveDir == Vec3::Zero)
		return;

	m_runtimeDesc.vCurDir = vMoveDir;

	pTransform->Look_At_Dir(vMoveDir);

	const _float fSpeed = pTransform->Get_MovePerSec();
	Vec3 vDisp = vMoveDir * fSpeed * fTimeDelta;
	Vec3 vPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	if (CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>())
	{
		pRigidBody->Move(vPos + vDisp, fTimeDelta);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos + vDisp);
		m_runtimeDesc.fTravelDistance += fSpeed * fTimeDelta;
		return;
	}

	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos + vDisp);
	m_runtimeDesc.fTravelDistance += vDisp.Length();
}

void CSkillObject_Base::Process_Life(const _float fTimeDelta)
{
	if (Has_Flag(ESkillObjectFlag::Life_Timer))
	{
		if (m_runtimeDesc.Life.Tick(fTimeDelta))
		{
			// Set_Dead
			Set_Dead();
			return;
		}
	}
	if (Has_Flag(ESkillObjectFlag::Life_Distance))
	{
		if (m_runtimeDesc.fTravelDistance >= m_desc.fMaxDistance)
		{
			// Set_Dead
			Set_Dead();
			return;
		}
	}
}

Vec3 CSkillObject_Base::Compute_MoveDir(const _float fTimeDelta, const Vec3& vCurDir)
{
	Vec3 vDir = vCurDir;
	if (Has_Flag(ESkillObjectFlag::Move_Return) && Has_Requester())
	{
		Vec3 vRequesterPosition = m_desc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vMyPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		vDir = vRequesterPosition - vMyPos;
	}
	else if (Has_Flag(ESkillObjectFlag::Move_Homing) && Has_Target())
	{
		Vec3 vTargetPos = m_desc.pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vMyPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vDesired = vTargetPos - vMyPos;
		if (vDesired != Vec3::Zero)
		{
			vDesired.Normalize();
			vDir = Vec3::Lerp(vDir, vDesired, fTimeDelta * m_desc.fHomingStrength);
		}
	}
	else if (Has_Flag(ESkillObjectFlag::Move_Straight))
	{
		//
	}
	else
		return Vec3::Zero;

	if (vDir != Vec3::Zero)
		vDir.Normalize();

	return vDir;
}

void CSkillObject_Base::Free()
{
	Super::Free();
}