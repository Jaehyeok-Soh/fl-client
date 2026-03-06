#include "pch.h"
#include "SkillBase_MoonQ.h"

#include "Player.h"
#include "StatCom_Player.h"
#include "SingleSkillSpawner.h"

#include "GameInstance.h"

CSkillBase_MoonQ::CSkillBase_MoonQ()
	:Super()
{
}

HRESULT CSkillBase_MoonQ::Initialize(void* pArg)
{
	SKILL_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 12.f;
	tMyDesc.FSkillFlags = Sheild_Add | SkillAtt_Add | Mental_Sub  | Sheild_Sub | SkillAtt_Sub;

	SKILL_INFO tSkill = {};
	{
		tSkill.fSkillAttack = 10.f;

		tSkill.fStatAttack = 0.f;
		tSkill.fStatSheild = 10.f;
		tSkill.eSkillType = SKILL_TYPE::BUFF;
		tSkill.fNeedMental = 35.f;

		TIME_COUNTER tCoolTimer = {};
		{
			tCoolTimer.bTimeReset = false;
			tCoolTimer.fMaxTime = 3.5f;
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

void CSkillBase_MoonQ::Awake(const _uint iCurLevelIndex)
{
	m_pAttackSkill_ObjSpawner->Awake(iCurLevelIndex);
}	

void CSkillBase_MoonQ::Update_Default(const _float fTimeDelta, CMyStat* pStatCom)
{
	m_pAttackSkill_ObjSpawner->Update(fTimeDelta);
}

void CSkillBase_MoonQ::Update(const _float fTimeDelta, CMyStat* pStatCom )
{
	Super::Update(fTimeDelta, pStatCom);

	// defense 계속 더해줌
	static_cast<CStatCom_Player*>(pStatCom)->Add_Stat(CMyStat::STAT_TYPE::DEFENSE, fTimeDelta * 2.f);

	if (!m_bSkillAttackOn)
	{
		m_TAttackSkillObj_Timer.x += fTimeDelta;

		if (m_TAttackSkillObj_Timer.x >= m_TAttackSkillObj_Timer.y)
		{
			Spawn_Attack_SkillObj(pStatCom);
			m_bSkillAttackOn = true;
			m_TAttackSkillObj_Timer.x = 0.f;
		}
	}
}

_bool CSkillBase_MoonQ::Start_Skill(CMyStat* pStatCom)
{
	if (Super::Start_Skill(pStatCom))
	{
		static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::Q, true);
		static_cast<CStatCom_Player*>(pStatCom)->Set_Attack_AddRate(m_fAddAttackRate);
		// 충돌체 발사

		m_bSkillAttackOn = false;
		m_TAttackSkillObj_Timer.x = 0.f;

		return true;
	}

	return false;
}

void CSkillBase_MoonQ::End_Skill(CMyStat* pStatCom)
{
	Super::End_Skill(pStatCom);
	static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::Q, false);

	// 충돌체 회수
}

_bool CSkillBase_MoonQ::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return false;
}

void CSkillBase_MoonQ::Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat)
{
	tStat_ExtraDesc.fAddRate = 0.15f;
}

HRESULT CSkillBase_MoonQ::Ready_Spawner()
{

	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = 0;
		desc.iSpawnLevelIndex = 0;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			0, g_wszSpawner_MoonSkillQ_Attack, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pAttackSkill_ObjSpawner = static_cast<CSingleSkillSpawner*>(pResult);
	}

	return S_OK;
}

void CSkillBase_MoonQ::Spawn_Attack_SkillObj(CMyStat* pOwnerStat)
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;

	CTransform* pPlayerTrans = pOwnerStat->Get_Owner()->Get_Component<CTransform>();

	// attack skill obj trigger
	{
		desc.vOrigin = pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::POS);
		m_pAttackSkill_ObjSpawner->Trigger(desc);
	}
}

void CSkillBase_MoonQ::Update_Skill(const _float fTimeDelta, CMyStat* pStatCom)
{

}

CSkillBase_MoonQ* CSkillBase_MoonQ::Create(void* pArg)
{
	CSkillBase_MoonQ* pInstance = new CSkillBase_MoonQ();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillBase_MoonQ::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBase_MoonQ::Free()
{
	Super::Free();

	Safe_Release(m_pAttackSkill_ObjSpawner);
}