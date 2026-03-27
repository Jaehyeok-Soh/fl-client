#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)
class CState_Npctalk final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Npctalk(CActionState* pOwnerComponent);
	virtual ~CState_Npctalk() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	virtual _bool Can_CheckKey(const _float fTimeDelta) override;

private:
	Vec3 m_vPivot = { Vec3::Zero };

	DelegateHandle m_DDialogHandle = {};
	DelegateHandle m_DInteractObjHandle = {};

private:
	void Bind_Event(const _uint iCurrentLevelID);

	void Change_State_byKeyInput();

public:
	static CState_Npctalk* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
