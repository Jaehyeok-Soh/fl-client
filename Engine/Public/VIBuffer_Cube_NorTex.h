#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube_NorTex final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CVIBuffer_Cube_NorTex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Cube_NorTex(const CVIBuffer_Cube_NorTex& rhs);
	virtual ~CVIBuffer_Cube_NorTex() = default;

	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Cube_NorTex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END