#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIPlayerStat_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagUIPlayerStatProgressDesc : public PROGRESS_BAR_DESC
	{
	}PLAYER_STAT_PROGRESS_DESC;

private:
	CUIPlayerStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIPlayerStat_Progress(const CUIPlayerStat_Progress& rhs);
	virtual ~CUIPlayerStat_Progress() = default;

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
	void Initialize_Visible_Event()override;
	void Initialize_InVisible_Event()override;
	_bool Tick_Visible_Event(const _float fTimeDelta)override;
	_bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(PLAYER_STAT_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	HRESULT Convert_Stat_To_Ratio();
private:
	// Player HP Func
	void Low_HP(const _float fTimeDelta);

private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };
	CGameObject* m_pTargetMonster = { nullptr };

	// Player HP Values
	_bool m_isStartLowHp	= { FALSE };
	_bool m_isEndLowHp		= { FALSE };
	_float m_fTickTimeAcc	= {};
	Vec4 m_vOriginColor		= {};
	Vec4 m_vOriginGradiantColor = {};
	Vec4 m_vLowHpColor		= {};
	_bool m_isHPPulse		= {};

public:
	static CUIPlayerStat_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END