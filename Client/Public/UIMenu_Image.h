#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUIMenu_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIMenuImageDesc : public DIMAGE_DESC
	{
	}MENU_IMAGE_DESC;

private:
	CUIMenu_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMenu_Image(const CUIMenu_Image& rhs);
	virtual ~CUIMenu_Image() = default;
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
	HRESULT Ready_Components(MENU_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
private:
	Vec2 m_vOriginPos = {};
	Vec2 m_vTargetPos = {};
public:
	static CUIMenu_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END