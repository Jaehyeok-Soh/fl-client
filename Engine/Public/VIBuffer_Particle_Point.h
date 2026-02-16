#pragma once
#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_Particle
{
	using Super = CVIBuffer_Particle;
public:
	typedef struct tagVIBuffer_Particle_Point_Desc : public CVIBuffer_Particle::PARTICLE_ORIGIN_DESC
	{

	}PARTICLE_POINT_ORIGIN_DESC;
private:
	CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs);
	virtual ~CVIBuffer_Particle_Point() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Bind_Resource() override;
	virtual void Render() override;
	virtual HRESULT Resize_InstanceBuffer(const PARTICLE_ORIGIN_DESC& Desc) override;
	virtual void Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& desc) override;

public:
	virtual HRESULT Set_VertexBuffer(const PARTICLE_ORIGIN_DESC& Desc);
	//virtual HRESULT Set_ResizeBuffer_UseRandomSeed();
	//virtual HRESULT Set_ResizeBuffer_NoneUseRandomSeed();
	virtual HRESULT Set_ResizeBuffer_SpecificRandom();
	virtual HRESULT Set_InstanceBuffer();

public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END