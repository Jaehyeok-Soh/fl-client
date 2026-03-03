#pragma once
#include "SkillObject_Base.h"

NS_BEGIN(Client)
class CMoon_SkillE_Obj final : public CSkillObject_Base
{
	using Super = CSkillObject_Base;
private:
	CMoon_SkillE_Obj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMoon_SkillE_Obj(const CMoon_SkillE_Obj& rhs);
	virtual ~CMoon_SkillE_Obj() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;
private:
	HRESULT Ready_Components();
public:
	static CMoon_SkillE_Obj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END