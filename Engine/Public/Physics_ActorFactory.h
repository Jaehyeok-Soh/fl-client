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
    vector<PxRigidActor*> GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);

private:
    vector<PxRigidActor*> MakeStatics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
    vector<PxRigidActor*> MakeDynamics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
    vector<PxRigidActor*> MakeKinematics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);

    PxRigidActor* MakeStatic(const Matrix world, Vec3 scale, vector<PxShape*>& shapes);
    PxRigidActor* MakeDynamic(const Matrix world, Vec3 scale, _float density, vector<PxShape*>& shapes);
    PxRigidActor* MakeKinematic(const Matrix world, Vec3 scale, _float density, vector<PxShape*>& shapes);

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