#pragma once
#include "StateMonster_ComboBase.h"

NS_BEGIN(Client)

class CStateMonster_Combo final : public CStateMonster_ComboBase
{
	using Super = CStateMonster_ComboBase;
public:
	typedef struct tagMonsterComboDesc : public MONSTER_COMBOBASE_DESC
	{
		vector<string> vecComboAnimName;

		_uint iEndStateIndex = {};

	}MONSTERCOMBO_DESC;

private:
	CStateMonster_Combo(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CStateMonster_Combo() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CStateMonster_Combo* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END