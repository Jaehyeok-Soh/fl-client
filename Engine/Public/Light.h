#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
	using Super = CBase;
private:
	explicit CLight();
	virtual ~CLight() = default;

	HRESULT Initialize(const LIGHT_DESC& LightDesc);
public:
	LIGHT_TYPE Get_Type() const { return m_eType; }
	const SHADER_LIGHTDESC &Get_LightDesc() const { return m_tLightDesc; }

	void Setup_Position(const _float4& vPosition) { m_tLightDesc.vPosition = vPosition; }
	void Setup_Range(const _float fRange) { m_tLightDesc.fRange = fRange; }
	void Setup_Diffuse(const _float4& vDiffuse) { m_tLightDesc.vDiffuse = vDiffuse; }
	void Setup_Ambient(const _float4& vAmbient) { m_tLightDesc.vAmbient = vAmbient; }
private:
	LIGHT_TYPE m_eType = { LIGHT_TYPE::END };
	SHADER_LIGHTDESC				m_tLightDesc = {};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

NS_END