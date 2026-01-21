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
public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END