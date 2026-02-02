#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CPhysics_LandScape final : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagPXLandScapeDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrColliderPrototypeName = { L"" };
	}PXLANDSCAPE_DESC;
private:
	CPhysics_LandScape(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPhysics_LandScape(const CPhysics_LandScape& rhs);
	virtual ~CPhysics_LandScape() = default;

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Components(PXLANDSCAPE_DESC* desc);
	HRESULT Ready_Physics(PXLANDSCAPE_DESC* desc);
	HRESULT Ready_PhysicsCollider(wstring prototypeTag);
	HRESULT Ready_PhysicsRigidBody();

public:
	static CPhysics_LandScape* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END