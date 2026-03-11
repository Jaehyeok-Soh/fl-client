#include "Engine_pch.h"
#include "SkillObjectBase.h"
#include "EffectBase.h"
#include "ColliderModule.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

// 1. Effect OwnerMatrix 세팅은 언제?

CSkillObjectBase::CSkillObjectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CSkillObjectBase::CSkillObjectBase(const CSkillObjectBase& rhs)
	: Super(rhs)
{
}

HRESULT CSkillObjectBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillObjectBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_pMyMatrix = Get_Component<CTransform>()->Get_WorldMatrixPtr();
	m_tDefaultEffectDesc.pTransformMatrix = &m_pMyMatrix;
	m_tDefaultEffectDesc.iSimulationType = ENUM_TO_UINT(EFFECT_SPAWN_DESC::E_VFX_SIMULTYPE::VFX_LOCAL);
	return S_OK;
}

HRESULT CSkillObjectBase::Spawn_FromPool(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	SKILLOBJECT_SPAWN_DESC* pSpawnDesc = static_cast<SKILLOBJECT_SPAWN_DESC*>(pArg);

	// 리셋
	m_tDesc = *pSpawnDesc;
	m_iFlag = m_tDesc.iFlags;
	Reset_RuntimeDesc();

	// SpawnPos 세팅
	CTransform* pTransform = Get_Component<CTransform>();
	CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, m_tDesc.vSpawnPos);
	if (pRigidBody)
		pRigidBody->SetTransform(pTransform->Get_WorldMatrix());

	// 파생 override 호출
	On_Spawned(*pSpawnDesc);
	Start_HybridState(m_iCurrentState);
	Enable_StateModules(m_iCurrentState);
	return S_OK;
}

HRESULT CSkillObjectBase::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	On_Despawned();

	// 리셋
	m_tDesc = {};
	m_tRuntimeDesc = {};
	m_iFlag = 0;
	return S_OK;
}

void CSkillObjectBase::On_EffectModuleEnter(CGameObject* pModule)
{
	if (CEffectBase* pEffect = dynamic_cast<CEffectBase*>(pModule))
		pEffect->Enable_VFX(&m_tDefaultEffectDesc);
}

void CSkillObjectBase::On_EffectModuleExit(CGameObject* pModule)
{
	if (CEffectBase* pEffect = dynamic_cast<CEffectBase*>(pModule))
		pEffect->Disable_VFX();
}

void CSkillObjectBase::On_ColliderModuleEnter(CGameObject* pModule)
{
	if (CColliderModule* pCollider = dynamic_cast<CColliderModule*>(pModule))
		pCollider->Enable(this);
}

void CSkillObjectBase::On_ColliderModuleExit(CGameObject* pModule)
{
	if (CColliderModule* pCollider = dynamic_cast<CColliderModule*>(pModule))
		pCollider->Disable();
}

void CSkillObjectBase::On_ModuleEnter(EHybridModuleType eType, CGameObject* pModule)
{
	Sync_ModuleWorldToOwner(pModule);

	switch (eType)
	{
	case EHybridModuleType::EFFECT:
		Compute_Rotate(pModule);
		On_EffectModuleEnter(pModule);
		break;
	case EHybridModuleType::COLLIDER:
		On_ColliderModuleEnter(pModule);
		break;
	default:
		break;
	}	
}

void CSkillObjectBase::On_ModuleExit(EHybridModuleType eType, CGameObject* pModule)
{
	switch (eType)
	{
	case EHybridModuleType::EFFECT:  
		On_EffectModuleExit(pModule);
		break;
	case EHybridModuleType::COLLIDER:
		On_ColliderModuleExit(pModule);
		break;
	default:
		break;
	}
}

void CSkillObjectBase::LoopEnd_EffectModules(_uint iState)
{
	auto itr = m_mapStateModules.find(iState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		if (Pair.first != EHybridModuleType::EFFECT)
			continue;

		CEffectBase* pEffect{ nullptr };
		if ((pEffect = dynamic_cast<CEffectBase*>(Pair.second)) == nullptr)
			continue;

		pEffect->LoopStateChange(DTO::E_LoopState::LOOP_END);
	}
}

void CSkillObjectBase::Process_Move(const _float fTimeDelta)
{
	CTransform* pTransform = Get_Component<CTransform>();
	if (Has_Flag(ESkillObjectFlag::Follow_Owner) && Is_RequesterAlive())
	{
		const Vec3 vReqPos = m_tDesc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vReqPos + m_tRuntimeDesc.vFollowOffset);
		return;
	}

	const Vec3 vMoveDir = Compute_MoveDir(fTimeDelta, m_tRuntimeDesc.vCurDir);
	if (vMoveDir == Vec3::Zero)
		return;

	m_tRuntimeDesc.vCurDir = vMoveDir;

	pTransform->Look_At_Dir(vMoveDir);

	const _float fSpeed = pTransform->Get_MovePerSec();
	Vec3 vDisp = vMoveDir * fSpeed * fTimeDelta;
	Vec3 vPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	_float fLength = vDisp.Length();

	if (CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>())
	{
		pRigidBody->Move(vPos + vDisp, fTimeDelta);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos + vDisp);
		m_tRuntimeDesc.fTravelDistance += fLength;
		return;
	}

	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos + vDisp);
	m_tRuntimeDesc.fTravelDistance += fLength;
}

void CSkillObjectBase::Process_Life(const _float fTimeDelta)
{
	if (IsDying() || IsDead())
		return;

	if (Has_Flag(ESkillObjectFlag::Life_Timer))
	{
		// 수명시간이 다되서 플래그가 true가 된다면
		if (m_tRuntimeDesc.Life.Tick(fTimeDelta) == true)
		{
 			Set_Dying();
			// 파생 클래스의 On_LifeOver를 호출
			On_LifeOver();
		}
	}
	if (Has_Flag(ESkillObjectFlag::Life_Distance))
	{
		if (m_tRuntimeDesc.fTravelDistance >= m_tDesc.fMaxDistance)
		{
			Set_Dying();
			// 파생 클래스의 On_LifeOver를 호출
			On_LifeOver();
			return;
		}
	}
}

Vec3 CSkillObjectBase::Compute_MoveDir(const _float fTimeDelta, const Vec3& vCurDir)
{
	Vec3 vDir = vCurDir;
	if (Has_Flag(ESkillObjectFlag::Move_Return) && Is_RequesterAlive())
	{
		Vec3 vRequesterPosition = m_tDesc.pRequester->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vMyPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		vDir = vRequesterPosition - vMyPos;
	}
	else if (Has_Flag(ESkillObjectFlag::Move_Homing) && Is_TargetAlive())
	{
		Vec3 vTargetPos = m_tDesc.pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vMyPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vDesired = vTargetPos - vMyPos;
		if (vDesired != Vec3::Zero)
		{
			vDesired.Normalize();
			vDir = Vec3::Lerp(vDir, vDesired, fTimeDelta * m_tDesc.fHomingStrength);
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

_bool CSkillObjectBase::Is_LifeOver() const
{
	// 수명이 다됬다면
	if (Has_Flag(ESkillObjectFlag::Life_Timer) && (m_tRuntimeDesc.Life.Is_Active() == false))
		return true;

	// 이동거리가 MaxDistance를 넘어섰다면
	if (Has_Flag(ESkillObjectFlag::Life_Distance)
		&& (m_tRuntimeDesc.fTravelDistance >= m_tDesc.fMaxDistance))
		return true;

	return false;
}

_bool CSkillObjectBase::Is_StateEffectFinished(_uint iState) const
{
	auto itr = m_mapStateModules.find(iState);
	// 없으면 완료 취급
	if (itr == m_mapStateModules.end())
		return true;
	
	for (const auto& [type, pGo] : itr->second)
	{
		if (type != EHybridModuleType::EFFECT || pGo == nullptr)
			continue;

		const CEffectBase* pEffect = dynamic_cast<const CEffectBase*>(pGo);
		if (pEffect == nullptr)
			continue;

		if (pEffect->IsEffectFinished() == false)
			return false;
	}

	return true;
}

void CSkillObjectBase::Sync_ModuleWorldToOwner(CGameObject* pModel)
{
	if (pModel == nullptr)
		return;

	CTransform* pTransform = Get_Component<CTransform>();
	CTransform* pModuleTransform = pModel->Get_Component<CTransform>();
	CPhysicsRigidBody* pModuleRigidBody = pModel->Get_Component<CPhysicsRigidBody>();
	pModuleTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());

	if (pModuleRigidBody)
		pModuleRigidBody->SetTransform(pTransform->Get_WorldMatrix());
}

void CSkillObjectBase::Reset_RuntimeDesc()
{
	m_tRuntimeDesc = {};
	m_tRuntimeDesc.vStartPos = m_tDesc.vSpawnPos;
	m_tRuntimeDesc.vCurDir = m_tDesc.vDirection;

	if (m_tRuntimeDesc.vCurDir != Vec3::Zero)
		m_tRuntimeDesc.vCurDir.Normalize();

	if (m_tDesc.vLookDir != Vec3::Zero)
		m_tDesc.vLookDir.Normalize();

	m_tRuntimeDesc.Life.Start(m_tDesc.fLifeTime);

	if (Has_Flag(ESkillObjectFlag::Follow_Owner) && Is_RequesterAlive())
	{
		const Vec3 vReqPos = m_tDesc.pRequester
			->Get_Component<CTransform>()
			->Get_Info(TRANSFORM_INFO_STATE::POS);
		m_tRuntimeDesc.vFollowOffset = m_tDesc.vSpawnPos - vReqPos;
	}
}

void CSkillObjectBase::Compute_Rotate(CGameObject* pEffectModule)
{
	if (pEffectModule == nullptr)
		return;
	if (m_tDesc.eEffectRotateState >= TRANSFORM_INFO_STATE::POS)
		return;

	CTransform* pTransform = Get_Component<CTransform>();
	CTransform* pEffectTransform = pEffectModule->Get_Component<CTransform>();

	Vec3 vAxis = pTransform->Get_Info(m_tDesc.eEffectRotateState);
	if (vAxis.LengthSquared() < g_XMEpsilon.f[0])
		return;

	vAxis.Normalize();
	// 03/10 Effefct는 matworld를 넣어서 내부적으로 offset처리가되고있다 ..? 때문에 Transform 소용없음...?
	// pEffectTransform->Rotation(vAxis, ::XMConvertToRadians(m_tDesc.fEffectDegree));
	Matrix matRotation = Matrix::CreateFromAxisAngle(vAxis, ::XMConvertToRadians(m_tDesc.fEffectDegree));
	m_tDefaultEffectDesc.matWorld = matRotation;
}

void CSkillObjectBase::Free()
{
	Super::Free();
}
