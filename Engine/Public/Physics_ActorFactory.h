#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPhysics_ActorFactory final : public CBase
{
	using Super = CBase;
private:
	CPhysics_ActorFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
	virtual ~CPhysics_ActorFactory() = default;

	HRESULT Initialize();

public:
    PxRigidActor* GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);

private:
    PxRigidActor* MakeStatic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
    PxRigidActor* MakeDynamic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
    PxRigidActor* MakeKinematic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };

public:
    static CPhysics_ActorFactory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
    virtual void Free();
};

NS_END