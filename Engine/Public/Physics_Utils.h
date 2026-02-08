#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;

class CPhysics_Utils final : public CBase
{
	using Super = CBase;
private:
	CPhysics_Utils(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
	virtual ~CPhysics_Utils() = default;

	HRESULT Initialize();

#ifdef _DEBUG
public:
    HRESULT Render(PxRigidActor* pActor, XMVECTOR color = DirectX::Colors::White);
    HRESULT Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color = DirectX::Colors::White);
#endif

public:
    PxTransform XMMatrixToPxTransform(Matrix mat);
    Matrix PxTransformToXMMatrix(PxTransform pxTransform);

    PxQuat GetPureRotation(const Matrix& mat);
    PxVec3 GetPureScale(const Matrix& mat);

    Matrix GetUnrealMatrix(const Matrix& mat);

public:
    _bool RayCast();

    _bool Execute_Overlap(PxGeometry& shape, PxTransform transform, OUT PxOverlapBuffer hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback);

    class CPhysics_QueryFilterCallback* GetQueryFilterCallback();

private:
    class CGameInstance* m_pGameInstance = { nullptr };

    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

#ifdef _DEBUG
private:
    PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    ID3D11InputLayout* m_pInputLayout = { nullptr };
    const PxU32 m_iMaxRenderShape = { 20 };
    ID3D11DepthStencilState* m_pDSS = { nullptr };
#endif

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };

    PxRaycastBuffer m_RayCastHitBuffer = {};
    _bool m_bRayHit = { false };

    PxMaterial* mtrlEx = { nullptr };

    const Matrix g_UnrealToEngineBasis = Matrix(
        1.f, 0.f, 0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    );

public:
    static CPhysics_Utils* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
    virtual void Free();
};

NS_END