#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Color final : public CVIBuffer
{
	using Super = CVIBuffer;
public:
	typedef struct tagVIBufferRectDesc : public Super::VIBUFFER_ORIGIN_DESC
	{
		Vec4 vColor = { 0.f, 1.f, 0.f, 1.f };
	}VIBUFFER_RECT_ORIGIN_DESC;
private:
	CVIBuffer_Rect_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Rect_Color(const CVIBuffer_Rect_Color& rhs);
	virtual ~CVIBuffer_Rect_Color() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
private:
	Vec4 m_vColor = { 0.f, 1.f, 0.f, 1.f };
public:
	static CVIBuffer_Rect_Color* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END