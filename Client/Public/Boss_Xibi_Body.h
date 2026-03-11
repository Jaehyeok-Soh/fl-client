#pragma once
#include "Monster_Body_Base.h"

NS_BEGIN(Client)

class CBoss_Xibi_Body final : public CMonster_Body_Base
{
	using Super = CMonster_Body_Base;
private:
	CBoss_Xibi_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBoss_Xibi_Body(const CBoss_Xibi_Body& rhs);
	virtual ~CBoss_Xibi_Body() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	static CBoss_Xibi_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void *pArg) override;
	virtual void Free() override;
};

NS_END