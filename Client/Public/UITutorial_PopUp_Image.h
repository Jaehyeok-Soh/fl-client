#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)
class CPlayer;
class CUITutorial_PopUp_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUITutorialPopUpImageDesc : public DIMAGE_DESC
	{
		_uint iTutorialTypeID = {};
	}TUTORIAL_POPUP_IMAGE_DESC;
private:
	CUITutorial_PopUp_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUITutorial_PopUp_Image(const CUITutorial_PopUp_Image& rhs);
	virtual ~CUITutorial_PopUp_Image() = default;
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
	HRESULT Ready_Components(TUTORIAL_POPUP_IMAGE_DESC* pDesc);
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
	_bool m_isSpawned = { false };

	DelegateHandle m_tEventHandle = {};

	EUITutorialPopUpTypeID m_eTutorialTypeID = { EUITutorialPopUpTypeID::END };

	_float m_fOriginWidth = {};

	_bool m_isFirstEntered = { false };
	_bool m_isFirstTriggered = { false };

	_bool m_isTriggered = { false };

	CPlayer* m_pPlayer = { nullptr };

	//GARA
	_bool m_isPannel = { false };

public:
	static CUITutorial_PopUp_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END