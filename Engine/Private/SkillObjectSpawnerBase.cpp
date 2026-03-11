#include "Engine_pch.h"
#include "SkillObjectSpawnerBase.h"
#include "Model.h"
#include "Transform.h"
#include "SkillObjectBase.h"
#include "GameInstance.h"

CSkillObjectSpawnerBase::CSkillObjectSpawnerBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CSkillObjectSpawnerBase::CSkillObjectSpawnerBase(const CSkillObjectSpawnerBase& rhs)
	: Super(rhs)
	, m_pOriginDesc(rhs.m_pOriginDesc)
{
}

HRESULT CSkillObjectSpawnerBase::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	if (pDesc)
		m_pOriginDesc = new SPAWNER_ORIGIN_DESC(*pDesc);
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillObjectSpawnerBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
		m_desc = *static_cast<SPAWNER_COPY_DESC*>(pArg);
	else
		return E_FAIL;

	return S_OK;
}

void CSkillObjectSpawnerBase::Spawn_SkillObject(const Vec3& vSpawnPos, const Vec3& vDir)
{
	Spawn_SkillObject(vSpawnPos, vDir, vDir);
}

void CSkillObjectSpawnerBase::Spawn_SkillObject(const Vec3& vSpawnPos, const Vec3& vMoveDir, const Vec3& vLookDir)
{
	if (m_pOriginDesc->wstrSkillPoolTag.empty())
		return;

	CSkillObjectBase::SKILLOBJECT_SPAWN_DESC desc{};
	desc.pRequester = m_desc.pRequester;
	desc.pTarget = m_desc.pTarget;

	desc.iFlags = m_pOriginDesc->iSkillObjectFlags;
	desc.fLifeTime = m_pOriginDesc->fLifeTime;
	desc.fMaxDistance = m_pOriginDesc->fMaxDistance;
	desc.fHomingStrength = m_pOriginDesc->fHomingStrength;

	desc.vSpawnPos = vSpawnPos;
	desc.vDirection = vMoveDir;
	desc.vLookDir = vLookDir;

	// eunbi 초반에 셋팅한 속도 == transform speed
	desc.fSpeed = m_pOriginDesc->fSpeed;

	// 이펙트 회전관련
	if ((std::abs)(m_desc.fEffect_Rotation_Degree) > g_XMEpsilon.f[0])
	{
		desc.eEffectRotateState = m_desc.eEffectRotationState;
		desc.fEffectDegree = m_desc.fEffect_Rotation_Degree;
	}

	CGameInstance::GetInstance()->Request_AddObject(
		m_pOriginDesc->iPoolLevelIndex, m_pOriginDesc->wstrSkillPoolTag,
		m_pGameInstance->Get_CurrentLevelIndex(), &desc);
}

HRESULT CSkillObjectSpawnerBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CTransform* pTransform = Get_Component<CTransform>();
	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, m_desc.vOrigin);

	m_eState = EState::Idle;
	m_fStateElapsed = 0.f;
	m_fSpawnAcc = 0.f;
	m_iSpawnedCount = 0;
	Start_State(m_eState);
	return S_OK;
}

void CSkillObjectSpawnerBase::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CSkillObjectSpawnerBase::Update(const _float fTimeDelta)
{
	if (m_eState == EState::Idle)
		return;

	Super::Update(fTimeDelta);
	Update_State(fTimeDelta);
}

void CSkillObjectSpawnerBase::Update_Late(const _float fTimeDelta)
{
	if (m_eState == EState::Idle)
		return;

	Super::Update_Late(fTimeDelta);
}

void CSkillObjectSpawnerBase::Ready_Before_Render(const _float fTimeDelta)
{
	if (m_eState == EState::Idle)
		return;

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CSkillObjectSpawnerBase::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSkillObjectSpawnerBase::Trigger(const SPAWNER_COPY_DESC& desc, _bool bForceRestart)
{
	if (m_eState != EState::Idle && !bForceRestart)
		return;

	if (bForceRestart && m_eState != EState::Idle)
		End_State(m_eState);

	m_desc = desc;
	CTransform* pTransform = Get_Component<CTransform>();
	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, m_desc.vOrigin);

	m_eState = EState::Spawn;
	m_iSpawnedCount = 0;
	m_fSpawnAcc = 0.f;
	m_fStateElapsed = 0.f;
	Start_State(m_eState);
}

void CSkillObjectSpawnerBase::Change_State(EState eState)
{
	if (m_eState == eState)
		return;

	End_State(m_eState);
	m_fStateElapsed = 0.f;
	m_eState = eState;
	Start_State(m_eState);
}

void CSkillObjectSpawnerBase::Start_State(EState eState)
{
	switch (eState)
	{
	case Engine::CSkillObjectSpawnerBase::EState::Spawn:
		Start_Spawn();
		break;
	default:
		break;
	}
}

void CSkillObjectSpawnerBase::Update_State(const _float fTimeDelta)
{
	m_fStateElapsed += fTimeDelta;

	switch (m_eState)
	{
	case Engine::CSkillObjectSpawnerBase::EState::Spawn:
		Update_Spawn(fTimeDelta);
		break;
	default:
		break;
	}
}

void CSkillObjectSpawnerBase::End_State(EState eState)
{
	switch (eState)
	{
	case Engine::CSkillObjectSpawnerBase::EState::Spawn:
		End_Spawn();
		break;
	default:
		break;
	}
}

void CSkillObjectSpawnerBase::Start_Spawn()
{
	m_fSpawnAcc = m_pOriginDesc->fInterval;
}

void CSkillObjectSpawnerBase::Update_Spawn(const _float fTimeDelta)
{
	if (m_fStateElapsed < m_pOriginDesc->fStartDelay)
		return;

	const _uint iTotal = Get_TotalCount();
	if (iTotal == 0)
	{
		Change_State(EState::Idle);
		return;
	}

	const Vec3 vForward = Get_Forward();
	const Vec3 vUp = Get_Up();

	// 발사 람다
	auto EmitOne = [&]()
	{
		Emit_One(m_iSpawnedCount, vForward, vUp);
		++m_iSpawnedCount;
	};

	if (m_pOriginDesc->fInterval <= 0.f)
	{
		const _uint iLeft = iTotal - m_iSpawnedCount;
		const _uint iBatch = (std::min)(iLeft, m_pOriginDesc->iMaxPerTick);

		for (_uint i = 0; i < iBatch && m_iSpawnedCount < iTotal; ++i)
			EmitOne();
	}
	else
	{
		m_fSpawnAcc += fTimeDelta;
		while (m_fSpawnAcc >= m_pOriginDesc->fInterval && m_iSpawnedCount < iTotal)
		{
			m_fSpawnAcc -= m_pOriginDesc->fInterval;
			EmitOne();
		}
	}

	if (m_iSpawnedCount >= iTotal)
		Change_State(EState::Idle);
}

void CSkillObjectSpawnerBase::End_Spawn()
{
}

Vec3 CSkillObjectSpawnerBase::Get_Forward()
{
	Vec3 vForward = m_desc.vForward;
	if (vForward== Vec3::Zero)
		vForward = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	if (vForward == Vec3::Zero)
		vForward = Vec3(0.f, 0.f, 1.f);

	vForward.Normalize();
	return vForward;
}

Vec3 CSkillObjectSpawnerBase::Get_Up() const
{
	Vec3 vUp = m_desc.vUp;
	if (vUp == Vec3::Zero)
		vUp = Vec3(0.f, 1.f, 0.f);

	vUp.Normalize();
	return vUp;
}

void CSkillObjectSpawnerBase::Free()
{
	if (IsClone() == false)
		Safe_Delete(m_pOriginDesc);

	Super::Free();
}