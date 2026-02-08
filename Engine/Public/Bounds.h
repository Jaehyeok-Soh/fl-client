#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CBounding_AABB;
class CBounding_Sphere;

typedef struct tagBounds
{
	CBounding_AABB* pAABB{ nullptr };
	CBounding_Sphere* pSphere{ nullptr };
}MESH_BOUNDS;

class ENGINE_DLL CBounds : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::BOUND;
	typedef struct tagBoundComponentDesc
	{
		const Vec3* pMinMax{ nullptr };
		_float fRatio{ 1.f };
	}BOUND_COMP_DESC;
private:
	CBounds(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBounds(const CBounds& rhs);
	virtual ~CBounds() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update_BoundingDesc(const Matrix& matWorld);
	_bool IntersectWith_Frustrum(BoundingFrustum* pFrustrum);
	_bool IntersectWithRay_World(OUT Vec3& vOut);
	_bool IntersectWithRay_Local(OUT Vec3& vOut);
	BoundingBox* Get_WolrdAABB();
private:
	MESH_BOUNDS m_tBounds{};
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
public:
	static CBounds* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void *pArg) override;
	virtual void Free() override;
#ifdef _DEBUG
public:
	virtual void Render() override;
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif
};

NS_END