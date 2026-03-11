#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CShader;

class ENGINE_DLL CRenderFx final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::RENDER_FX;
private:
	enum class EFlag : _uint
	{
		None = 0,
		Emissive = 1 << 0,
		Shake = 1 << 1
	};
	enum class EEmissiveMode : _uint
	{
		None = 0,
		Static,
		Pulse
	};
public:
	typedef struct tagRenderFxCopyDesc
	{
		Vec3   vEmissiveColor = {};
		_float fEmissiveDefaultIntensity = { 0.f };

		_float fShakeAmpX = 0.f;
		_float fShakeAmpY = 0.f;
		_float fShakeFreq = 0.f;
		_float fShakePhase = 0.f;
	}RENDER_FX_COPY_DESC;
	typedef struct tagRenderFxRuntimeDesc
	{
		TIME_LINE tTimer{};
	}RENDER_FX_RUNTIME_DESC;
	typedef struct tagRenderFxEmissiveRuntimeDesc
	{
		EEmissiveMode eMode = EEmissiveMode::None;
		TIME_LINE tTimer{};
		_float fCurrentIntensity = 0.f;
		_float fPulsePeakIntensity = -1.f;
		_float fFadeIn = 0.f;
		_float fHold = 0.f;
		_float fFadeOut = 0.f;
	}RENDER_FX_EMISSIVE_RUNTIME_DESC;
private:
	CRenderFx(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CRenderFx(const CRenderFx& rhs);
	virtual ~CRenderFx() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void *pArg) override;
public:
	void Update(const _float fTimeDelta);
	// Emissive
	void Change_EmissiveColor(const Vec3& vColor) { m_tDesc.vEmissiveColor = vColor; }
	void Enable_EmissiveStatic();
	void Play_EmissivePulse(_float fFadeIn, _float fHold, _float fFadeOut);
	void Clear_Emissive();

	// Shake
	void Set_ShakeSetting(_float fAmpX, _float fAmpY, _float fFreq, _float fPhase = 0.f);
	void Play_Shake(_float fDuration);
	void Stop_Shake();

	// Shader Bind
	HRESULT Bind_Resources(Engine::CShader* pShader);
public:
	void Update_Shake(const _float fTimeDelta);
	void Update_Emissive(const _float fTimeDelta);
	void Rebuild_ShaderDesc();
private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
	RENDER_FX_COPY_DESC m_tDesc{};
	RENDER_FX_RUNTIME_DESC m_tRuntimeDesc{};
	RENDER_FX_EMISSIVE_RUNTIME_DESC m_tEmissiveRuntimeDesc{};
	SHADER_RENDER_FX_DESC m_tShaderDesc{};
public:
	static CRenderFx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END