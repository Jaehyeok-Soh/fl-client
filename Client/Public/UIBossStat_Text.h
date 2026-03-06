#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"
NS_BEGIN(Engine)
class CMyStat;
NS_END
NS_BEGIN(Client)
class CWorldUI_Component;
class CUIBossStat_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUIBossStatTextDesc : public UI_TEXT_DESC
	{
	}BOSS_STAT_TEXT_DESC;

private:
	CUIBossStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIBossStat_Text(const CUIBossStat_Text& rhs);
	virtual ~CUIBossStat_Text() = default;

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
	HRESULT Ready_Components(BOSS_STAT_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Attach_Personal_Info();
	HRESULT Convert_Stat_To_Text();
private:
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;
private:
	CMyStat* m_pTargetStat = { nullptr };
	CWorldUI_Component* m_pWorldUIComp = { nullptr };
	_bool m_isSpawned = { false };

public:
	static CUIBossStat_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END