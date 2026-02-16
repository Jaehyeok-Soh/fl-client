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

		const Vec3*		pMinMax{ nullptr };
		_float			fRatio{ 1.f };

	}BOUND_COMP_DESC;
private:
	CBounds(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBounds(const CBounds& rhs);
	virtual ~CBounds() = default;

	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;

public:
	/* 기본 Boudns를 생성해주는함수 InstanceMesh는 Sub가 개별역할으해주고 m_tBounds 는 단일 콜라이더로 인스턴스 매쉬들의 전체영역을 min Max 잡아서 그 영역을 지정한다 */
	HRESULT				Make_Bounds(const Vec3* pMinMax , float fRatio = 1.f);

	/* 일정 Index의 SubBounds를 Update해주는함수 */
	HRESULT				Update_SubBound(const Vec3* pModelMinMax , const Matrix& WorldMatrix , _uint iIndex);

	HRESULT				Add_SubBounds(const Vec3* pMinMax, span<Matrix> spanInstanceMatrix, _float fRatio = 1.f);
	
	/* 기존에 있던 배열에 추가 해주는 함수 */
	HRESULT				Push_SubBounds(const Vec3* pTotalMinMax, const Vec3* pModelMinMax, const Matrix& WorldMatrix , float fRatio = 1.f);
	HRESULT				Delete_SubBounds(const Vec3* pTotalMinMax , _uint iDeleteIndex , float fRatio = 1.f);


	void				Update_BoundingDesc(const Matrix& matWorld);
	_bool				IntersectWith_Frustrum(BoundingFrustum* pFrustrum);
	void				IntersectWith_Frustrum_SubBounds(BoundingFrustum* pFrustrum,OUT vector<_uint> &vecVisibleIndex);
	_bool				IntersectWithRay_World(OUT Vec3& vOut, OUT _int& iIndex);
	_bool				IntersectWithRay_Local(OUT Vec3& vOut, OUT _int& iIndex);
	BoundingBox*		Get_WolrdAABB();

private:
	CBounding_Sphere*	Create_Sphere(Vec3* pMinMax);
	CBounding_AABB*		Create_AABB(Vec3* pMinMax);
	void				Clear_SubBounds();
private:
	MESH_BOUNDS			m_tBounds{};
	vector<MESH_BOUNDS> m_vecSubBounds;
	ID3D11Device*		m_pDevice{ nullptr };
	ID3D11DeviceContext*m_pDeviceContext{ nullptr };
public:
	static CBounds*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void *pArg) override;
	virtual void		Free() override;
#ifdef _DEBUG
public:
	virtual void		Render() override;
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif
};

NS_END