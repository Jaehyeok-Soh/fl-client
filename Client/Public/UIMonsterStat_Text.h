#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"

NS_BEGIN(Engine)
class CMyStat;
NS_END

NS_BEGIN(Client)

class CUIMonsterStat_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUIMonsterStatDesc : public UI_TEXT_DESC
	{
	}MONSTER_STAT_DESC;

private:
	CUIMonsterStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMonsterStat_Text(const CUIMonsterStat_Text& rhs);
	virtual ~CUIMonsterStat_Text() = default;

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
	HRESULT Ready_Components(MONSTER_STAT_DESC* pDesc);
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
	CGameObject* m_pTargetMoster = { nullptr };
	CMyStat* m_pTargetStat = { nullptr };

	_bool m_isSpawned = { false };

public:
	static CUIMonsterStat_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END