#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)
class CUIScreenPulse_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIScrennPulseImageDesc : public DIMAGE_DESC
	{
	}SCREENPULSE_IMAGE_DESC;
private:
	CUIScreenPulse_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIScreenPulse_Image(const CUIScreenPulse_Image& rhs);
	virtual ~CUIScreenPulse_Image() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(SCREENPULSE_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	// Glow Effect Values
	Vec2	m_vNoiseUVScale = {};
	Vec2	m_vNoiseUVScroll = {};
	_float	m_fTime = {};
	_float	m_fGlowDistort = {};
	_float	m_fGlowPulseSpeed = {};
	_float	m_fGlowIntensity = {};

	_bool m_isVisibleNow= { false };
public:
	static CUIScreenPulse_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END