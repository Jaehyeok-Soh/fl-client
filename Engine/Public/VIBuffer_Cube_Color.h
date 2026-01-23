#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube_Color final : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagVIBufferCubeOriginDesc : public Super::VIBUFFER_ORIGIN_DESC
	{
		Vec4 vColor = { 0.f, 1.f, 0.f, 1.f };
	}VIBUFFER_CUBE_ORIGIN_DESC;
private:
	CVIBuffer_Cube_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Cube_Color(const CVIBuffer_Cube_Color& rhs);
	virtual ~CVIBuffer_Cube_Color() = default;

	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	Vec4 m_vColor = { 0.f, 1.f, 0.f, 1.f };
public:
	static CVIBuffer_Cube_Color* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END