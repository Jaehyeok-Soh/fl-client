#pragma once
#include "NPC_Body_Base.h"

NS_BEGIN(Client)

class CNPC_Kid_Body_1 final : public CNPC_Body_Base
{
	using Super = CNPC_Body_Base;
private:
	CNPC_Kid_Body_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNPC_Kid_Body_1(const CNPC_Kid_Body_1& rhs);
	virtual ~CNPC_Kid_Body_1() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(NPCBODY_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static CNPC_Kid_Body_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END