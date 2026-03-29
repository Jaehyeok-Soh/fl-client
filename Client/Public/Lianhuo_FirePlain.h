#pragma once
#include "SkillWarningSpace.h"

NS_BEGIN(Client)

class CLianhuo_FirePlain final : public CSkillWarningSpace
{
	using Super = CSkillWarningSpace;
private:
	CLianhuo_FirePlain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLianhuo_FirePlain(const CLianhuo_FirePlain& rhs);
	virtual ~CLianhuo_FirePlain() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Modules();
public:
	static CLianhuo_FirePlain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END