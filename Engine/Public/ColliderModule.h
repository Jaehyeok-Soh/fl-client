#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CColliderModule final : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagColliderModuleDesc : public Super::GAMEOBJECT_DESC
	{
		PHYSICSCOLLIDER_DESC* pPhysicsColliderDesc{};
		PHYSICSRIGIDBODY_DESC* pPhysicsRigidbodyDesc{};
		CGameObject* pOwner{ nullptr };
	}COLLIDERMODULE_COPY_DESC;
private:
	CColliderModule(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderModule(const CColliderModule& rhs);
	virtual ~CColliderModule() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	void Enable(CGameObject* pOwner);
	void Disable();
	virtual HRESULT Awake(_uint iCurrentLevelIndex) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual void OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo) final override;
	virtual void OnTrigger_Exit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther) final override;
private:
	CGameObject* m_pOwner = { nullptr };
public:
	static CColliderModule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END