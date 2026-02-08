#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Engine)

class CGravityForce :
    public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	explicit CGravityForce(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGravityForce(const CGravityForce& rhs);
	virtual ~CGravityForce() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize_Prototype() override { return S_OK; } // Mono Behavior
	virtual HRESULT Initialize(void* pArg) override;  // Mono Behavior
	virtual void	Update(const _float fTimeDelta) override {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

public:
	static CGravityForce* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END