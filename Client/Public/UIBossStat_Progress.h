#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CWorldUI_Component;
class CStatCom_Boss;
class CUIBossStat_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagUIBossStatProgressDesc : public PROGRESS_BAR_DESC
	{
	}BOSS_STAT_PROGRESS_DESC;

private:
	CUIBossStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIBossStat_Progress(const CUIBossStat_Progress& rhs);
	virtual ~CUIBossStat_Progress() = default;

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
	HRESULT Ready_Components(BOSS_STAT_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	void Initialize_Visible_Event()override;
	void Initialize_InVisible_Event()override;
	_bool Tick_Visible_Event(const _float fTimeDelta)override;
	_bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;

	HRESULT Convert_Stat_To_Ratio();

private:
	CStatCom_Boss* m_pTargetStat = { nullptr };
	_bool m_isSpawned = { false };
	Vec4 m_vOriginColorTint = {};

	_bool m_isBossGroggyTrigger = { false };

public:
	static CUIBossStat_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END