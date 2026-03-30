#pragma once
#include "SkillWarningSpace.h"

NS_BEGIN(Client)

class CLianhuo_ChainThron final : public CSkillWarningSpace
{
	using Super = CSkillWarningSpace;
private:
	CLianhuo_ChainThron(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLianhuo_ChainThron(const CLianhuo_ChainThron& rhs);
	virtual ~CLianhuo_ChainThron() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	virtual void Build_WarningDesc() override;
private:
	HRESULT Ready_Modules();
public:
	static CLianhuo_ChainThron* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END