#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CColliderModule final : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagColliderModuleDesc
	{
		PHYSICSCOLLIDER_DESC* pPhysicsColliderDesc{};
		PHYSICSRIGIDBODY_DESC* pPhysicsRigidbodyDesc{};
	}COLLIDERMODULE_COPY_DESC;
private:
	CColliderModule(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderModule(const CColliderModule& rhs);
	virtual ~CColliderModule() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	void Enable();
	void Disable();
	virtual HRESULT Awake(_uint iCurrentLevelIndex) override;
private:

public:
	static CColliderModule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END