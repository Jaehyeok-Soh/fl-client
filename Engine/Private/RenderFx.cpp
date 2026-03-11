#include "Engine_pch.h"
#include "RenderFx.h"
#include "Shader.h"
#include "GameInstance.h"

CRenderFx::CRenderFx(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CRenderFx::CRenderFx(const CRenderFx& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderFx::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderFx::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		RENDER_FX_COPY_DESC* pDesc = static_cast<RENDER_FX_COPY_DESC*>(pArg);
		m_tDesc = *pDesc;
	}

	return S_OK;
}

void CRenderFx::Update(const _float fTimeDelta)
{
	Update_Shake(fTimeDelta);
	Update_Emissive(fTimeDelta);
	Rebuild_ShaderDesc();
}

void CRenderFx::Enable_EmissiveStatic()
{
	m_tEmissiveRuntimeDesc = {};
	m_tEmissiveRuntimeDesc.eMode = EEmissiveMode::Static;
	m_tEmissiveRuntimeDesc.fCurrentIntensity = m_tDesc.fEmissiveDefaultIntensity;
}

void CRenderFx::Play_EmissivePulse(_float fFadeIn, _float fHold, _float fFadeOut)
{
	m_tEmissiveRuntimeDesc = {};
	m_tEmissiveRuntimeDesc.eMode = EEmissiveMode::Pulse;
	m_tEmissiveRuntimeDesc.fFadeIn = (fFadeIn < 0.f) ? 0.f : fFadeIn;
	m_tEmissiveRuntimeDesc.fHold = (fHold < 0.f) ? 0.f : fHold;
	m_tEmissiveRuntimeDesc.fFadeOut = (fFadeOut < 0.f) ? 0.f : fFadeOut;
	m_tEmissiveRuntimeDesc.fPulsePeakIntensity = m_tDesc.fEmissiveDefaultIntensity;

	m_tEmissiveRuntimeDesc.tTimer.Start(
		m_tEmissiveRuntimeDesc.fFadeIn
		+ m_tEmissiveRuntimeDesc.fHold
		+ m_tEmissiveRuntimeDesc.fFadeOut);
}

void CRenderFx::Clear_Emissive()
{
	m_tEmissiveRuntimeDesc = {};
}

void CRenderFx::Set_ShakeSetting(_float fAmpX, _float fAmpY, _float fFreq, _float fPhase)
{
	m_tDesc.fShakeAmpX = fAmpX;
	m_tDesc.fShakeAmpY = fAmpY;
	m_tDesc.fShakeFreq = fFreq;
	m_tDesc.fShakePhase = fPhase;
}

void CRenderFx::Play_Shake(_float fDuration)
{
	m_tRuntimeDesc.tTimer.Start(fDuration);
}

void CRenderFx::Stop_Shake()
{
	m_tRuntimeDesc.tTimer.Clear();
}

HRESULT CRenderFx::Bind_Resources(CShader* pShader)
{
	if (pShader == nullptr)
		return E_FAIL;
	return pShader->Bind_RenderFxData(m_tShaderDesc);
}

void CRenderFx::Update_Shake(const _float fTimeDelta)
{
	if (m_tRuntimeDesc.tTimer.Is_Active() == false)
		return;

	m_tRuntimeDesc.tTimer.Tick(fTimeDelta);
}

void CRenderFx::Update_Emissive(const _float fTimeDelta)
{
	switch (m_tEmissiveRuntimeDesc.eMode)
	{
	case EEmissiveMode::None:
		m_tEmissiveRuntimeDesc.fCurrentIntensity = 0.f;
		break;

	case EEmissiveMode::Static:
		m_tEmissiveRuntimeDesc.fCurrentIntensity = m_tDesc.fEmissiveDefaultIntensity;
		break;
	case EEmissiveMode::Pulse:
	{
		m_tEmissiveRuntimeDesc.tTimer.Tick(fTimeDelta);

		const _float fT = m_tEmissiveRuntimeDesc.tTimer.fElapsed;
		const _float fFadeIn = m_tEmissiveRuntimeDesc.fFadeIn;
		const _float fHold = m_tEmissiveRuntimeDesc.fHold;
		const _float fFadeOut = m_tEmissiveRuntimeDesc.fFadeOut;
		const _float fPeak = m_tEmissiveRuntimeDesc.fPulsePeakIntensity;

		const _float t0 = fFadeIn;
		const _float t1 = fFadeIn + fHold;
		const _float t2 = fFadeIn + fHold + fFadeOut;

		_float fIntensity = 0.f;

		if (fT < t0)
		{
			const _float denom = (fFadeIn > g_XMEpsilon.f[0]) ? fFadeIn : 1.f;
			const _float alpha = (std::clamp)(fT / denom, 0.f, 1.f);
			fIntensity = (1.f - alpha) * 0.f + alpha * fPeak;
		}
		else if (fT < t1)
		{
			fIntensity = fPeak;
		}
		else if (fT < t2)
		{
			const _float denom = (fFadeOut > g_XMEpsilon.f[0]) ? fFadeOut : 1.f;
			const _float alpha = (std::clamp)((fT - t1) / denom, 0.f, 1.f);
			fIntensity = (1.f - alpha) * fPeak + alpha * 0.f;
		}
		else
		{
			Clear_Emissive();
			return;
		}
		m_tEmissiveRuntimeDesc.fCurrentIntensity = fIntensity;
	}
	break;
	}
}

void CRenderFx::Rebuild_ShaderDesc()
{
	m_tShaderDesc = {};

	// Emissive
	if (m_tEmissiveRuntimeDesc.fCurrentIntensity > g_XMEpsilon.f[0])
	{
		Engine_Utils::Add_Flag(m_tShaderDesc.iFlags, ENUM_TO_UINT(EFlag::Emissive));
		m_tShaderDesc.vEmissiveColor = m_tDesc.vEmissiveColor;
		m_tShaderDesc.fEmissiveIntensity = m_tEmissiveRuntimeDesc.fCurrentIntensity;
	}

	// Shake
	if (m_tRuntimeDesc.tTimer.Is_Active())
	{
		Engine_Utils::Add_Flag(m_tShaderDesc.iFlags, ENUM_TO_UINT(EFlag::Shake));
		const _float t = m_tRuntimeDesc.tTimer.fElapsed;
		const _float wave = XM_2PI * m_tDesc.fShakeFreq * t + m_tDesc.fShakePhase;

		m_tShaderDesc.fOffsetX = sinf(wave) * m_tDesc.fShakeAmpX;
		m_tShaderDesc.fOffsetY = cosf(wave * 1.13f) * m_tDesc.fShakeAmpY;
	}
}

CRenderFx* CRenderFx::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderFx* pInstance = new CRenderFx(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CRenderFx::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRenderFx::Clone(void* pArg)
{
	CRenderFx* pInstance = new CRenderFx(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CRenderFx::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderFx::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}
