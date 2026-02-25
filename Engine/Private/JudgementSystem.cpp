#include "Engine_pch.h"
#include "JudgementSystem.h"
#include "GameObject.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

CJudgementSystem::CJudgementSystem()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CJudgementSystem::Initialize()
{
	m_vecPending.reserve(2000);
	return S_OK;
}

void CJudgementSystem::Flush_CollidedEvent()
{
	for (const auto& desc : m_vecPending)
	{
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
	m_vecPending.clear();
}

void CJudgementSystem::Clear()
{
	m_vecPending.clear();
}

void CJudgementSystem::Process_Trigger(const COLLIDED_DESC& desc)
{
	if (desc.pOther->IsDead() || desc.pRequester->IsDead())
		return;

}

void CJudgementSystem::Process_Battle(const COLLIDED_DESC& desc)
{
	if (desc.pOther->IsDead() || desc.pRequester->IsDead())
		return;

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