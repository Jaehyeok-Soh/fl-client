#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsAttackRaycast final : public CComponent
{
public:
	typedef struct tagAttackRaycastDesc
	{
		PHYSICSFILTERGROUP::Enum eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
		_uint iFilterMask = {};
		string strAttackPresetTag = { "" };
		_uint iAttackPresetID = { UINT_MAX };
	}ATTACKRAYCASTDESC;

private:
	using Super = CComponent;

public:
	constexpr static EComponentType _ID = EComponentType::PX_ATTACKRAYCAST;

private:
	CPhysicsAttackRaycast(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPhysicsAttackRaycast(const CPhysicsAttackRaycast& rhs);
	virtual ~CPhysicsAttackRaycast() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	virtual void Render() override;
#endif

public:
	_bool Aimming(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, _float* hitDist);
	_bool ShootRay(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, _float* hitDist);

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	PxScene* m_pScene = { nullptr };
	class CPhysics_QueryFilterCallback_Gun* m_pFilterCallback = { nullptr };
	PxQueryFilterData m_filterData = { PxQueryFilterData() };

	ATTACKRAYCASTDESC m_tDesc{};

	PxRaycastBuffer m_RayCastHitBuffer = {};
	_bool m_bRayHit = { false };


public:
	static CPhysicsAttackRaycast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END