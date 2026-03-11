#include "pch.h"
#include "SkillBase_MoonE.h"

#include "Player.h"
#include "StatCom_Player.h"
#include "SingleSkillSpawner.h"

#include "GameInstance.h"

CSkillBase_MoonE::CSkillBase_MoonE()
	:Super()
{
}

HRESULT CSkillBase_MoonE::Initialize(void* pArg)
{
	// 10 0
	SKILL_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 5.f;
	tMyDesc.FSkillFlags =  Attack_Add | Mental_Sub | Attack_Sub;

	SKILL_INFO tSkill = {};
	{
		tSkill.fStatAttack = 10.f;
		tSkill.fStatSheild = 0.f;
		tSkill.eSkillType = SKILL_TYPE::DAMAGE;
		tSkill.fNeedMental = 15.f;

		TIME_COUNTER tCoolTimer = {};
		{
			tCoolTimer.bTimeReset = false;
			tCoolTimer.fMaxTime = 0.f;
		}
		tSkill.tCoolTimer = tCoolTimer;
	}
	tMyDesc.tSkillInfo = tSkill;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	if (FAILED(Ready_Spawner()))
		return E_FAIL;

	return S_OK;
}

void CSkillBase_MoonE::Awake(const _uint iCurLevelIndex)
{
	m_pSkillObjSpawner->Awake(iCurLevelIndex);
}

void CSkillBase_MoonE::Update_Default(const _float fTimeDelta, CMyStat* pStatCom)
{
	m_pSkillObjSpawner->Update(fTimeDelta);
}

void CSkillBase_MoonE::Update(const _float fTimeDelta, CMyStat* pStatCom)
{
	Super::Update(fTimeDelta, pStatCom);

	m_fAccTime += fTimeDelta;

	if (!m_bSpawn_First)
	{
		// 0.5초 지났다면 생성
		if (m_fAccTime >= 0.3f)
		{
			Spawn_SkillObj(pStatCom, true);
			m_bSpawn_First = true;
		}
		return;
	}

	// 아직 sceond skill이 안 나갔다면
	else if (!m_bSpawn_Second)
	{
		// 0.5초 지났다면 생성
		if (m_fAccTime >= 0.5f)
		{
			Spawn_SkillObj(pStatCom, false);
			m_bSpawn_Second = true;
		}
	}
}

_bool CSkillBase_MoonE::Start_Skill(CMyStat* pStatCom)
{
	if (Super::Start_Skill(pStatCom))
	{
		static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::E, true);
		static_cast<CStatCom_Player*>(pStatCom)->Set_Critical_AddRate(1.f);

		m_bSpawn_First = false;
		m_bSpawn_Second = false;
		m_fAccTime = 0.f;

		return true;
	}

	return false;
}

void CSkillBase_MoonE::End_Skill(CMyStat* pStatCom)
{
	Super::End_Skill(pStatCom);

	static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::E, false);
	static_cast<CStatCom_Player*>(pStatCom)->Set_Critical_AddRate(0.f);
}

_bool CSkillBase_MoonE::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return false;
}

void CSkillBase_MoonE::Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat)
{
	static_cast<CStatCom_Player*>(pOwnerStat)->Set_Critical_AddRate(1.f);
}

void CSkillBase_MoonE::ClearSkillBase_WhenChangeLevel()
{
}

void CSkillBase_MoonE::Update_Skill(const _float fTimeDelta, CMyStat* pStatCom)
{
	Super::Update_Skill(fTimeDelta);
}

HRESULT CSkillBase_MoonE::Ready_Spawner()
{
	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = 0;
		desc.iSpawnLevelIndex = 0;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			0, g_wszSpawner_MoonSkillE, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pSkillObjSpawner = static_cast<CSingleSkillSpawner*>(pResult);
	}

	return S_OK;
}

void CSkillBase_MoonE::Spawn_SkillObj(CMyStat* pOwnerStat, _bool bFirst)
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;

	CTransform* pPlayerTrans = pOwnerStat->Get_Owner()->Get_Component<CTransform>();
	desc.vOrigin = pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vOrigin.y += 1.f;
	desc.vForward = pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK);

	desc.eEffectRotationState = TRANSFORM_INFO_STATE::LOOK;
	desc.fEffect_Rotation_Degree = bFirst ?  30.f : -30.f;

	m_pSkillObjSpawner->Trigger(desc);
} 

CSkillBase_MoonE* CSkillBase_MoonE::Create(void* pArg)
{
	CSkillBase_MoonE* pInstance = new CSkillBase_MoonE();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillBase_MoonE::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBase_MoonE::Free()
{
	Safe_Release(m_pSkillObjSpawner);

	Super::Free();
}