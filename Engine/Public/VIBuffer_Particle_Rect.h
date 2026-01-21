#pragma once
#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Rect final : public CVIBuffer_Particle
{
	using Super = CVIBuffer_Particle;
public:
	typedef struct tagVIBuffer_Particle_Rect_Desc : public Super::tagVIBuffer_ParticleOriginDesc
	{

	}PARTICLE_RECT_ORIGIN_DESC;
private:
	CVIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype);
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Particle_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END