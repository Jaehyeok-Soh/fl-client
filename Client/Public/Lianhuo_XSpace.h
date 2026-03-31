#pragma once
#include "SkillWarningSpace.h"

NS_BEGIN(Client)

class CLianhuo_XSpace final : public CSkillWarningSpace
{
	using Super = CSkillWarningSpace;
private:
	CLianhuo_XSpace(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLianhuo_XSpace(const CLianhuo_XSpace& rhs);
	virtual ~CLianhuo_XSpace() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Modules();
public:
	static CLianhuo_XSpace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END