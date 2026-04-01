#pragma once
#include "StateBase_Player.h"
#include "CameraMan_Targeter.h"

NS_BEGIN(Client)
class CState_Condemn final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Condemn(CActionState* pOwnerComponent);
	virtual ~CState_Condemn() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual _uint	Get_Capabilities() const override { return 0; } // ¹«Àû

private:
	CCameraMan_Targeter::TURNCAM_DATA m_tTurnData = {};

public:
	static CState_Condemn* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END