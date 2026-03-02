#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;

class CJudgementSystem final : public CBase
{
	using Super = CBase;
private:
	CJudgementSystem();
	virtual ~CJudgementSystem() = default;

	HRESULT Initialize();
public:
	void Push_CollidedData(const COLLIDED_DESC& desc) { m_vecPending_Write.push_back(desc); }
	void Flush_CollidedEvent();
	void Clear();
private:
	void Process_Trigger(const COLLIDED_DESC& desc);
	void Process_Battle(const COLLIDED_DESC& desc);
	void Process_Collision(const COLLIDED_DESC& desc);
	void Resolve_Attacker_Victim(const COLLIDED_DESC& desc, OUT CGameObject*& pOutAttacker, OUT CGameObject*& pOutVictim, OUT _uint &iOutAttackerLayer, OUT _uint &iOutVictimLayer);
	Vec3 Compute_HitNormal(CGameObject* pAttacker, CGameObject* pVictim, const Vec3& vRawNormal);

private:
	void Compute_FinalDamage(const DTO::TAttackPreset_Data* pAttackPreset, const EXTRA_ATTACK_DESC& tExtraDesc, OUT HIT_DESC &hitDesc);

private:
	_bool m_bFlushing{ false };
	vector<COLLIDED_DESC> m_vecPending_Write;
	vector<COLLIDED_DESC> m_vecPending_Read;
	CGameInstance* m_pGameInstance{ nullptr };
private:
	
public:
	static CJudgementSystem* Create();
	virtual void Free() override;
};

NS_END