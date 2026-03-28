#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUICombo_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUIComboTextDesc : public UI_TEXT_DESC
	{
	}COMBO_TEXT_DESC;

private:
	CUICombo_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUICombo_Text(const CUICombo_Text& rhs);
	virtual ~CUICombo_Text() = default;

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
	HRESULT Ready_Components(COMBO_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Attach_Personal_Info();
	void Tick_By_Type(const _float fTimeDelta)override;
	HRESULT Convert_Stat_To_Text();
	virtual void Bind_Events()override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

	void Convert_Count_To_Rank();

private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };

	ECombotype m_eCurComboType = { ECombotype::END };
	_uint m_iCurComboCount = {};
	_uint m_iPreComboCount = {};

	_bool m_isCountChange = { false };

public:
	static CUICombo_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END