#pragma once
#include "SkillPierceProjectile.h"

NS_BEGIN(Client)

class CXibi_Loop_Thunder : public CSkillPierceProjectile
{
	using Super = CSkillPierceProjectile;
private:
	CXibi_Loop_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CXibi_Loop_Thunder(const CXibi_Loop_Thunder& rhs);
	virtual ~CXibi_Loop_Thunder() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;
private:
	HRESULT Ready_Modules();
public:
	static CXibi_Loop_Thunder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END