#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

enum class E_PARTICLE_MOVESTATE
{
	NONE,
	DROP,
	RISE,
	SPREAD,
};

class ENGINE_DLL CVIBuffer_Particle abstract : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagVIBuffer_ParticleOriginDesc : public Super::tagVIBufferOriginDesc
	{
		_uint iInstnaceCount = { 0 };
		Vec2 vSize = { 0.f, 0.f };
		Vec3 vCenter = { 0.f, 0.f, 0.f };
		Vec3 vPivot = { 0.f, 0.f, 0.f };
		Vec3 vRange = { 0.f, 0.f, 0.f };
		Vec2 vSpeed = { 0.f, 0.f };
		Vec2 vLifeTime = { 0.f, 0.f };
		_bool isLoop = { false };
	}PARTICLE_ORIGIN_DESC;
protected:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& rhs);
	virtual ~CVIBuffer_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resource() override;
	virtual void Render() override;
	
public:
	void Update_Simulation(_float fTimeDelta, E_PARTICLE_MOVESTATE eType);
	void Reset_Simulation();

	// ======== 행동 패턴들 =========
	void Drop(_float fTimeDelta);
	void Rise(_float fTimeDelta);
	void Spread(_float fTimeDelta);

public:
	const PARTICLE_ORIGIN_DESC& Get_ParticleDesc() { return m_tParticleDesc; }
	void Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc);

	HRESULT Resize_InstanceBuffer(_uint iNumInstanceCount);

protected:
	_bool			m_bIsLoop = { false };
	_uint			m_iIndexCountPerInstance = {};
	_uint			m_iInstanceCount = {};
	_uint			m_iInstanceVertexStride = {};
	VTXPARTICLE*	m_pInstanceVertices = { nullptr };
	_float*			m_pSpeeds = { nullptr };
	Vec3			m_vPivot = {};
protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};
	PARTICLE_ORIGIN_DESC m_tParticleDesc = {};
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END