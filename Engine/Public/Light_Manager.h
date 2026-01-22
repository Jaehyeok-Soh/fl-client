#pragma once
#include "Light.h"

NS_BEGIN(Engine)

template<typename T>
class CConstant_Buffer;

class CLight_Manager final : public CBase
{
	using Super = CBase;
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLight_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Push_DynamicLight(class CLight* pLight);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect_Tex* pVIBuffer);
	ID3D11Buffer* Get_Light_ConstantBuffer() const;
	void Clear();
private:
	void Setup_Pass(class CShader* pShader, LIGHT_TYPE eType);
	void Setup_ConstantBuffer(const SHADER_LIGHTDESC& lightDesc);
private:
	list<CLight*>	m_Lights[ENUM_TO_UINT(LIGHT_TYPE::END)];

	CConstant_Buffer<SHADER_LIGHTDESC>* m_pLight_CBuffer = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	static CLight_Manager *Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END