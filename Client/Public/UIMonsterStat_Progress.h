#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Engine)
class CMyStat;
NS_END

NS_BEGIN(Client)
class CUIMonsterStat_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagUIMonsterStatProgress : public PROGRESS_BAR_DESC
	{
	}MONSTER_STAT_PROGRESS_DESC;

private:
	CUIMonsterStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMonsterStat_Progress(const CUIMonsterStat_Progress& rhs);
	virtual ~CUIMonsterStat_Progress() = default;

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
	HRESULT Ready_Components(MONSTER_STAT_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	void Initialize_Visible_Event()override;
	void Initialize_InVisible_Event()override;
	_bool Tick_Visible_Event(const _float fTimeDelta)override;
	_bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;

	HRESULT Convert_Stat_To_Ratio();

private:
	CGameObject* m_pTargetMoster = { nullptr };
	CMyStat* m_pTargetStat = { nullptr };

	// Monster HP Values
	_bool m_isStartLowHp = { FALSE };
	_bool m_isEndLowHp = { FALSE };
	_float m_fTickTimeAcc = {};
	Vec4 m_vOriginColor = {};
	Vec4 m_vOriginGradiantColor = {};
	Vec4 m_vLowHpColor = {};
	_bool m_isHPPulse = {};

public:
	static CUIMonsterStat_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END