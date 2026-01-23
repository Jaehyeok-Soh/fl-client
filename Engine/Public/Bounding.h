#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBounding abstract : public CBase
{
	using Super = CBase;
public:
	typedef struct tagBoundingDesc
	{
		Vec3 vCenter = {};
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CBounding() = default;

	HRESULT Initialize(BOUNDING_DESC* pDesc);
public:
	virtual void Update(const Matrix &matWorld) PURE;
	virtual _bool Intersect_Bounding(EColliderType eType, CBounding* pOther) PURE;
	virtual _bool IntersectWithRay_World(class CGameInstance * pGameInstance, OUT Vec3& vOut) PURE;
	virtual _bool IntersectWithRay_Local(class CGameInstance* pGameInstance, OUT Vec3&vOut) PURE;
	virtual _bool IntersectWithRay_World(class CRay* pRay, OUT Vec3& vOut) PURE;
	virtual _bool IntersectWithRay_Local(class CRay* pRay, OUT Vec3& vOut) PURE;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool bIsColl) PURE;
#endif
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	virtual void Free() override;
};

NS_END