#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBounding_OBB final : public CBounding
{
	using Super = CBounding;
public:
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3			vExtents;
		_float3			vAngles;
	}BOUNDING_OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBounding_OBB() = default;

	HRESULT Initialize(const BOUNDING_DESC* pInitialDesc);
public:
	BoundingOrientedBox* Get_OriginalDesc() { return m_pOriginalDesc; }
	BoundingOrientedBox* Get_Desc() { return m_pDesc; }
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect_Bounding(EColliderType eType, CBounding* pOther) override;
	virtual _bool IntersectWithRay_World(class CGameInstance* pGameInstance, OUT _float4& vOut) override;
	virtual _bool IntersectWithRay_Local(class CGameInstance* pGameInstance, OUT _float4& vOut) override;
	virtual _bool IntersectWithRay_World(class CRay* pRay, OUT _float4& vOut) override;
	virtual _bool IntersectWithRay_Local(class CRay* pRay, OUT _float4& vOut) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl) override;
#endif

private:
	BoundingOrientedBox* m_pOriginalDesc = { nullptr };
	BoundingOrientedBox* m_pDesc = { nullptr };
public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pInitialDesc);
	virtual void Free() override;
};

NS_END