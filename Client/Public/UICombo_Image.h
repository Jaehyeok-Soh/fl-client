#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUICombo_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIComboImageDesc : public DIMAGE_DESC
	{
	}COMBO_IMAGE_DESC;
private:
	CUICombo_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUICombo_Image(const CUICombo_Image& rhs);
	virtual ~CUICombo_Image() = default;
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
	HRESULT Ready_Components(COMBO_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

	void Convert_Count_To_Rank();
private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };

	// ÄÞº¸ ·©Å© ÀÌ¹ÌÁö Values 
	ECombotype m_eCurComboType	= { ECombotype::END };
	ECombotype m_ePreComboType	= { ECombotype::END };
	_uint m_iCurComboCount		= {};
	_uint m_iPreComboCount		= {};
	_bool m_isCountChange		= { false };
	_bool m_isComboChange		= { false };
	_float m_fComboCoolTime		= {};

	// Glow Effect Values
	Vec2	m_vNoiseUVScale		= {};
	Vec2	m_vNoiseUVScroll	= {};
	_float	m_fTime				= {};
	_float	m_fGlowDistort		= {};
	_float	m_fGlowPulseSpeed	= {};
	_float	m_fGlowIntensity	= {};

public:
	static CUICombo_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END