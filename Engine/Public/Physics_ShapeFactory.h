#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPhysics_ResourceManager;

class CPhysics_ShapeFactory final : public CBase
{
	using Super = CBase;
private:
	CPhysics_ShapeFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
	virtual ~CPhysics_ShapeFactory() = default;

	HRESULT Initialize(class CPhysics_ResourceManager* pResourceManager);

public:
    vector<PxShape*> GetShape(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxShape*> GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc);

private:
    vector<PxShape*> MakeShape(PHYSICSCOLLIDER_DESC* pDesc, vector<PxGeometryHolder> geometries);

    vector<PxGeometryHolder> MakeGeometry(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxGeometryHolder> MakeConvexGeometry(PHYSICSCOLLIDER_DESC* pDesc);

    vector<PxGeometryHolder> MakeTriangleMeshGeometry(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxGeometryHolder> MakeConvexMeshGeometry(PHYSICSCOLLIDER_DESC* pDesc);

    PxGeometryHolder MakeSphere(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeBox(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeCapsule(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakePlane(PHYSICSCOLLIDER_DESC* pDesc);

    PxGeometryHolder MakeConvexPoint(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeConvexSegment(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeConvexBox(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeConvexEllipsoid(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeConvexCylinder(PHYSICSCOLLIDER_DESC* pDesc);
    PxGeometryHolder MakeConvexCone(PHYSICSCOLLIDER_DESC* pDesc);

    vector<PxTriangleMesh*> GetTriangleMesh(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxConvexMesh*> GetConvexMesh(PHYSICSCOLLIDER_DESC* pDesc);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    CPhysics_ResourceManager* m_pResourceManager = { nullptr };

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };

public:
    static CPhysics_ShapeFactory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, class CPhysics_ResourceManager* pResourceManager);
    virtual void Free();
};

NS_END