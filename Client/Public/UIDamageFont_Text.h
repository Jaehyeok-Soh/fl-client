#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIDamageFont_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUIDamageFontTextDesc : public UI_TEXT_DESC
	{
	}DAMAGE_FONT_DESC;

private:
	CUIDamageFont_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIDamageFont_Text(const CUIDamageFont_Text& rhs);
	virtual ~CUIDamageFont_Text() = default;
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
	HRESULT Ready_Components(DAMAGE_FONT_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Convert_Stat_To_Text();
	HRESULT Tick_By_Type(const _float fTimeDelta);

private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };
public:
	static CUIDamageFont_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END