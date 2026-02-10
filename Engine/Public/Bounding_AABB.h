#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBounding_AABB final : public CBounding
{
	using Super = CBounding;
public:
	typedef struct tagBoundingAABBDesc : public Super::tagBoundingDesc
	{
		Vec3 vExtens = {};
		Vec3* pMinMax{ nullptr };
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBounding_AABB() = default;

	HRESULT Initialize(const BOUNDING_DESC* pInitializeDesc);
public:
	BoundingBox* Get_OriginalDesc() { return m_pOriginalDesc; }
	BoundingBox* Get_Desc() { return m_pDesc; }
	virtual void Update(const Matrix &matWorld) override;
	virtual _bool Intersect_Bounding(EColliderType eType, CBounding* pOther) override;
	virtual _bool IntersectWithRay_World(class CGameInstance* pGameInstance, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_Local(class CGameInstance* pGameInstance, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_World(class CRay* pRay, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_Local(class CRay* pRay, OUT Vec3& vOut) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool bIsColl) override;
#endif
private:
	BoundingBox* m_pOriginalDesc = { nullptr };
	BoundingBox* m_pDesc = { nullptr };
public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pInitializeDesc);
	virtual void Free() override;
};

NS_END