#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube_Tex final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CVIBuffer_Cube_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Cube_Tex(const CVIBuffer_Cube_Tex& rhs);
	virtual ~CVIBuffer_Cube_Tex() = default;

	virtual HRESULT Initialize_Prototype(void *pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Cube_Tex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END