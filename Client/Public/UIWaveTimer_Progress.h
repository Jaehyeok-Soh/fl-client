#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIWaveTimer_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagUIWaveTimerProgressDesc : public PROGRESS_BAR_DESC
	{
	}WAVE_TIMER_PROGRESS_DESC;
private:
	CUIWaveTimer_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIWaveTimer_Progress(const CUIWaveTimer_Progress& rhs);
	virtual ~CUIWaveTimer_Progress() = default;
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
	virtual void Bind_Events()override;
private:
	HRESULT Ready_Components(WAVE_TIMER_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	HRESULT Convert_Stat_To_Ratio();
private:
	class CTriggerBox_MonsterWaveSpawner* m_pSpawner = {};
public:
	static CUIWaveTimer_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END