#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)

class CUITitle_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUITitleTextDesc : public UI_TEXT_DESC
	{
	}TITLE_TEXT_DESC;

private:
	CUITitle_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUITitle_Text(const CUITitle_Text& rhs);
	virtual ~CUITitle_Text() = default;

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
	HRESULT Ready_Components(TITLE_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Attach_Personal_Info();
	HRESULT Convert_Stat_To_Text();
	virtual void Bind_Events()override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	_bool m_isTitleVisibleTrigger = { false };
public:
	static CUITitle_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END