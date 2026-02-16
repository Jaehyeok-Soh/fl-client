#pragma once
#include "VIBuffer_Particle.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CVIBuffer_Particle_Mesh final :
    public CVIBuffer_Particle
{
	using Super = CVIBuffer_Particle;
public:
	typedef struct tagVIBuffer_Particle_Mesh_Desc : public CVIBuffer_Particle::PARTICLE_ORIGIN_DESC
	{
	}PARTICLE_Mesh_ORIGIN_DESC;
private:
	CVIBuffer_Particle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Particle_Mesh(const CVIBuffer_Particle_Mesh& rhs);
	virtual ~CVIBuffer_Particle_Mesh() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Resize_InstanceBuffer(const PARTICLE_ORIGIN_DESC& Desc) override;
	virtual void Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& desc) override;

private:
	//virtual HRESULT Set_ResizeBuffer_UseRandomSeed();
	//virtual HRESULT Set_ResizeBuffer_NoneUseRandomSeed();
	virtual HRESULT Set_ResizeBuffer_SpecificRandom();

	virtual HRESULT Set_Instance_Buffer();
	virtual HRESULT Copy_IndexBuffer(class CMesh* pMesh);
	virtual HRESULT Copy_VertexBuffer(class CMesh* pMesh);

public:
	static CVIBuffer_Particle_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END