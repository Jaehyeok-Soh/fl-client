#pragma once
#include "VIBuffer.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CModel;
class CComputeShader;
class CGameObject;

// 추후에 여기에 Random Seed flag 값 들어올 예정.

class ENGINE_DLL CVIBuffer_Particle abstract : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagVIBuffer_ParticleOriginDesc : public Super::tagVIBufferOriginDesc
	{
		_float fDuration = { 1.f };
		_uint iInstnaceCount = { 0 };
		Vec3 vScale = { 0.f, 0.f, 0.f};
		Vec2 vSize = { 0.f, 0.f };
		Vec3 vCenter = { 0.f, 0.f, 0.f };
		Vec3 vPivot = { 0.f, 0.f, 0.f };
		Vec3 vRange = { 0.f, 0.f, 0.f };
		Vec2 vSpeed = { 0.f, 0.f };
		float m_fStartSpeeds = { 1.f };
		Vec2 vLifeTime = { 0.f, 0.f };
		_bool isLoop = { false };
		_uint iRandomFlags = { DTO::E_RANDOM_FLAG::RAND_NONE};
		CModel*	pModel = { nullptr };
		CGameObject* pOwner = { nullptr };
		CComputeShader* pComputeShader = { nullptr };
		_uint  EmissionFlagType = { 0 };
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
	virtual void Update_Simulation(CComputeShader* ComputeShader, Vec3 vLook, Vec3 finalGravity, _float fTImeDelta, _uint TimeFlag, DTO::E_SHAPETYPE eType);
	virtual void Reset_Simulation();

public:
	const PARTICLE_ORIGIN_DESC& Get_ParticleDesc() { return m_tParticleDesc; }
	virtual void Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc) {}
	virtual HRESULT Resize_InstanceBuffer(const PARTICLE_ORIGIN_DESC& Desc) { return S_OK; }

public:
	virtual _uint   Get_InstanceCount() { return m_iInstanceCount; }
	virtual _uint	Get_IndexCountPerInstance() { return m_iIndexCountPerInstance; }
	virtual void	Debug_CheckVertexBuffer();

protected:
	_bool			m_bIsLoop = { false };
	_uint			m_iIndexCountPerInstance = {};
	_uint			m_iInstanceCount = {};
	_uint			m_iInstanceVertexStride = {};
	VTXPARTICLE*	m_pInstanceVertices = { nullptr };
	_float*			m_pSpeeds = { nullptr };
	_float			m_fStartSpeeds = { 1.f };
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