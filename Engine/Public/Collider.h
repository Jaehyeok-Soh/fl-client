#pragma once
#include "Component.h"
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
	using Super = CComponent;
	static _uint s_iNextID;
public:
	typedef struct tagColliderDesc
	{
		_int iLayer = { -1 };
		CBounding::BOUNDING_DESC* pBoundingDesc = { nullptr };
	}COLLIDER_DESC;
public:
	constexpr static EComponentType _ID = EComponentType::COLLIDER;
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EColliderType eType);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update(const Matrix &matWorld);

	void OnCollision_Enter(CCollider* pOther);
	void OnCollision(CCollider* pOther);
	void OnCollision_Exit(CCollider* pOther);
	_bool Intersect(CCollider* pOther);
	_bool IntersectWithRay_World(OUT Vec3& vOut);
	_bool IntersectWithRay_Local(OUT Vec3& vOut);
	_bool IntersectWithRay_World(class CRay* pRay, OUT Vec3& vOut);
	_bool IntersectWithRay_Local(class CRay* pRay, OUT Vec3& vOut);
	CBounding* Get_Bounding() { return m_pBounding; }
	
	void Set_Active(_bool bActive) { m_bIsActive = bActive; }
	_bool Is_Active() const { return m_bIsActive; }

	EColliderType Get_Type() const { return m_eType; }
	_uint Get_ID() const { return m_iID; }
	_int Get_Layer() const { return m_iLayer; }
private:
	HRESULT Create_Bounding(CBounding::BOUNDING_DESC *pBoundingDesc);
#ifdef _DEBUG
public:
	virtual void Render() override;
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif
private:
	_bool m_bIsActive = { true };
	_bool m_bIsColl = { false };
	EColliderType m_eType = { EColliderType::END };
	_int m_iLayer = { -1 };
	_uint m_iID = { 0 };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CBounding* m_pBounding = { nullptr };
public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EColliderType eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END