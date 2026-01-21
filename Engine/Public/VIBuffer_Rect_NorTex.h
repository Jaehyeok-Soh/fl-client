#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_NorTex final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CVIBuffer_Rect_NorTex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Rect_NorTex(const CVIBuffer_Rect_NorTex& rhs);
	virtual ~CVIBuffer_Rect_NorTex() = default;

	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Rect_NorTex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END