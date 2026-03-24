#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CUIWeakness_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIWeaknessImageDesc : public DIMAGE_DESC
	{
	}WEAKNESS_IMAGE_DESC;

private:
	CUIWeakness_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIWeakness_Image(const CUIWeakness_Image& rhs);
	virtual ~CUIWeakness_Image() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	HRESULT Attach_Personal_Info();
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(WEAKNESS_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
private:
	Vec2 m_vOriginSize = {};

	// Glow Effect Values
	Vec2	m_vNoiseUVScale		= {};
	Vec2	m_vNoiseUVScroll	= {};
	_float	m_fTime				= {};
	_float	m_fGlowDistort		= {};
	_float	m_fGlowPulseSpeed	= {};
	_float	m_fGlowIntensity	= {};

public:
	static CUIWeakness_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END