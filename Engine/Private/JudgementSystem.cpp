#include "Engine_pch.h"
#include "JudgementSystem.h"
#include "GameObject.h"
#include "EngineConsole.h"
#include "GameInstance.h"

CJudgementSystem::CJudgementSystem()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CJudgementSystem::Initialize()
{
	m_vecPending_Write.reserve(1000);
	m_vecPending_Read.reserve(1000);
	return S_OK;
}

void CJudgementSystem::Flush_CollidedEvent()
{
	m_bFlushing = false;
	m_vecPending_Read.clear();
	m_vecPending_Read.swap(m_vecPending_Write);

	for (const auto& desc : m_vecPending_Read)
	{
		m_bFlushing = true;

		if (PHYSICSFILTERGROUP::IsAttackPair(desc.iRequesterLayer, desc.iOtherLayer))
		{
			Process_Battle(desc);
		}
		else if(PHYSICSFILTERGROUP::IsTriggerPair(desc.iRequesterLayer, desc.iOtherLayer))
		{
			Process_Trigger(desc);
		}
		else
		{
			Process_Collision(desc);
		}
	}

	m_bFlushing = false;
	m_vecPending_Read.clear();
}

void CJudgementSystem::Clear()
{
	m_vecPending_Write.clear();
	m_vecPending_Read.clear();
}

void CJudgementSystem::Process_Trigger(const COLLIDED_DESC& desc)
{
	if (desc.pOther->IsDead() || desc.pRequester->IsDead())
		return;
}

void CJudgementSystem::Process_Battle(const COLLIDED_DESC& desc)
{
	if (desc.pOther == nullptr || desc.pRequester == nullptr)
	{
		CLOG_ERROR("CJudgementSystem::Process_Battle, failed");
		return;
	}

	if (desc.pOther->IsDead() || desc.pRequester->IsDead())
		return;

	CGameObject* pAttacker{ nullptr };
	_uint iAttackerLayer{ UINT_MAX };
	CGameObject* pVictim{ nullptr };
	_uint iVictimLayer{ UINT_MAX };
	Resolve_Attacker_Victim(desc, pAttacker, pVictim, iAttackerLayer, iVictimLayer);


	if (pAttacker == nullptr || pVictim == nullptr)
	{
		CLOG_ERROR("CJudgementSystem::Process_Battle, Resolve failed");
		return;
	}

	const _uint iAttacker_PresetKey = (pAttacker == desc.pRequester)
		? desc.tHitInfo.iRequester_AttackPresetID
		: desc.tHitInfo.iOther_AttackPresetID;

	const auto* pPreset = m_pGameInstance->Find_AttackPrseet(iAttacker_PresetKey);

	if (pPreset == nullptr)
	{
		CLOG_ERROR("CJudgementSystem::Process_Battle, Find preset failed");
		return;
	}

	const Vec3 vFinalNormal = Compute_HitNormal(pAttacker, pVictim, desc.tHitInfo.vRawNormal);

	// ATTACKER_DESC
	ATTACKER_DESC attackerDesc{};
	{
		attackerDesc.iCollisionType = desc.iCollisionType;
		attackerDesc.iAttackerLayer = iAttackerLayer;
		attackerDesc.pAttacker = pAttacker;
		attackerDesc.pAttackPreset = pPreset;
	}
	// HIT_DESC
	HIT_DESC hitDesc{};
	{
		hitDesc.iVictimLayer = iVictimLayer;
		hitDesc.pVictim = pVictim;

		hitDesc.bHasHitPoint = desc.tHitInfo.bHasHitPoint;
		hitDesc.vHitPoint = desc.tHitInfo.vPosition;
		hitDesc.vHitNormal = vFinalNormal;
		hitDesc.fDepth = desc.tHitInfo.fDepth;
		hitDesc.attackDesc = attackerDesc;
	}

	Compute_FinalDamage(pPreset, desc.tExtraDesc, hitDesc);

	pVictim->On_Hit(hitDesc);
	pAttacker->Try_Attack(hitDesc);
}

void CJudgementSystem::Process_Collision(const COLLIDED_DESC& desc)
{
	if (desc.pOther->IsDead() || desc.pRequester->IsDead())
		return;

}

void CJudgementSystem::Resolve_Attacker_Victim(const COLLIDED_DESC& desc, OUT CGameObject*& pOutAttacker, OUT CGameObject*& pOutVictim, OUT _uint& iOutAttackerLayer, OUT _uint& iOutVictimLayer)
{
	const _bool bMyAttack = PHYSICSFILTERGROUP::IsAttackLayer(desc.iRequesterLayer);
	const _bool bOtherAttack = PHYSICSFILTERGROUP::IsAttackLayer(desc.iOtherLayer);

	// 요청자가 Attacker
	if (bMyAttack && bOtherAttack == false)
	{
		pOutAttacker = desc.pRequester;
		iOutAttackerLayer = desc.iRequesterLayer;

		pOutVictim = desc.pOther;
		iOutVictimLayer = desc.iOtherLayer;
		return;
	}

	// 상대가 Attacker
	if (bOtherAttack && bMyAttack == false)
	{
		pOutAttacker = desc.pOther;
		iOutAttackerLayer = desc.iOtherLayer;

		pOutVictim = desc.pRequester;
		iOutVictimLayer = desc.iRequesterLayer;
		return;
	}

	// 둘다 공격이거나 비공격이면 정보 밀어버리기
	CLOG_ERROR("CJudgementSystem::Resolve_Attacker_Victim, wrong type");
	pOutAttacker = nullptr;
	pOutVictim = nullptr;
	iOutAttackerLayer = 0;
	iOutVictimLayer = 0;
}

Vec3 CJudgementSystem::Compute_HitNormal(CGameObject* pAttacker, CGameObject* pVictim, const Vec3& vRawNormal)
{
	Vec3 vPos_Attacker = pAttacker->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vPos_Victim = pVictim->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vToVictim = vPos_Victim - vPos_Attacker;
	if (vToVictim.LengthSquared() <= g_XMEpsilon.f[0])
		vToVictim = Vec3(0.f, 1.f, 0.f);
	else
		vToVictim.Normalize();

	Vec3 vReturnNormal = vRawNormal;
	if (vReturnNormal.LengthSquared() <= g_XMEpsilon.f[0])
		return vToVictim;

	vReturnNormal.Normalize();

	// Attacker -> Victim으로 고정
	if (vReturnNormal.Dot(vToVictim) < 0.f)
		vReturnNormal *= -1.0f;

	return vReturnNormal;
}

void CJudgementSystem::Compute_FinalDamage(const DTO::TAttackPreset_Data* pAttackPreset, const EXTRA_ATTACK_DESC& tExtraDesc, OUT HIT_DESC& hitDesc)
{
	// base damage 우선 저장
	_float fDamege = pAttackPreset->tCombat.fBaseDamage;

	// damage flag는 피격쪽에서 정보 처리용으로 던지는 거기 때문에
	// 바로 값을 넣어준다
	hitDesc.iDamageFlag = tExtraDesc.iDamageFlag;

	// 만약 compute order 정보가 없다면 -> 바로 return 
	if (tExtraDesc.vecCompute_Order.empty())
	{
		hitDesc.fFinalDamage = fDamege;
		return;
	}

	// compute order 벡터를 순회하면서 순서대로 값을 계산해나감
	for (auto& pCompute : tExtraDesc.vecCompute_Order)
	{
		_float fCanCompute = 0.f;
		switch (pCompute)
		{
		case ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Add):
			fDamege += tExtraDesc.fAddDamage;
			break;

		case ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Rate):
			fDamege *= (1.f + tExtraDesc.fAddRate);
			break;

		case ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Add):
			fCanCompute = m_pGameInstance->Rand_Float(0.f, 1.f);
			if (fCanCompute < tExtraDesc.fRandomAdd_Rate)
			{
				fDamege += m_pGameInstance->Rand_Float(tExtraDesc.vRandomAdd_MinMax.x, tExtraDesc.vRandomAdd_MinMax.y);
			}
			break;

		case ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Rate):
			fCanCompute = m_pGameInstance->Rand_Float(0.f, 1.f);
			if (fCanCompute < tExtraDesc.fRandomMul_Rate)
			{
				fDamege *= (1.f + m_pGameInstance->Rand_Float(tExtraDesc.vRandomMul_MinMax.x, tExtraDesc.vRandomMul_MinMax.y));
			}
			break;
		}
	}

	// damage가 확 튀는 것을 막기 위해 clamp를 해준다
	if (tExtraDesc.vFinalDamege_MinMax.x <
		tExtraDesc.vFinalDamege_MinMax.y)
	{
		fDamege = std::clamp(fDamege,
			tExtraDesc.vFinalDamege_MinMax.x,
			tExtraDesc.vFinalDamege_MinMax.y);
	}

	hitDesc.fFinalDamage = fDamege;
}

CJudgementSystem* CJudgementSystem::Create()
{
	CJudgementSystem* pInstance = new CJudgementSystem();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CJudgementSystem::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudgementSystem::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}