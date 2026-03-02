#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBounding_Sphere final : public CBounding
{
	using Super = CBounding;
public:
	typedef struct tagBoundingSphereDesc : public CBounding::BOUNDING_DESC
	{
		_float			fRadius;
		Vec3*			pMinMax{ nullptr };
	}BOUNDING_SPHERE_DESC;
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBounding_Sphere( const CBounding_Sphere& rhs);
	virtual ~CBounding_Sphere() = default;

	HRESULT Initialize(const BOUNDING_DESC* pInitialDesc);
public:
	BoundingSphere* Get_OriginalDesc() { return m_pOriginalDesc; }
	BoundingSphere* Get_Desc() { return m_pDesc; }
	virtual void Update(const Matrix &WorldMatrix) override;
	virtual _bool Intersect_Bounding(EColliderType eType, CBounding* pOther) override;
	virtual _bool IntersectWithRay_World(class CGameInstance* pGameInstance, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_Local(class CGameInstance* pGameInstance, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_World(class CRay* pRay, OUT Vec3& vOut) override;
	virtual _bool IntersectWithRay_Local(class CRay* pRay, OUT Vec3& vOut) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl) override;
#endif
private:
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pInitialDesc);
	virtual void Free() override;
};

NS_END