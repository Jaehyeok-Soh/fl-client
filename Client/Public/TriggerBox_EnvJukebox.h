#pragma once
#include "TriggerBox.h"
#include "DataStruct_Map.h"
#include "Sound_Handler.h"

NS_BEGIN(Client)

class CTriggerBox_EnvJukebox final : public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagTriggerBox_EnvJukebox_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		ENV_JUKEBOX_TYPE::Enum eType = ENV_JUKEBOX_TYPE::RANGE;
		string strSoundTag = {};
		_uint iSoundHash = {};
		_float fRadius = {};
	}TRIGGERBOX_ENVJUKEBOX_DESC;
protected:
	CTriggerBox_EnvJukebox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_EnvJukebox(const CTriggerBox_EnvJukebox& rhs);
	virtual ~CTriggerBox_EnvJukebox() = default;

public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_ENVJUKEBOX_DESC* pDesc);

public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;

public:
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

public:
	virtual void			QuestEnter() override;
	virtual void			QuestExit() override;

private:
	void					Calc_PlayerDist();
	void					Ctrl_RangedVolume(_float fDist);

private:
	ENV_JUKEBOX_TYPE::Enum m_eType = ENV_JUKEBOX_TYPE::RANGE;
	string m_strSoundTag = {};
	_uint m_iSoundHash = {};
	_float m_fRadius = {};

	CSound_Handler::EFOOTSOUNDTYPE::Enum m_eEnvFootSoundType = { CSound_Handler::EFOOTSOUNDTYPE::NONE };

	_bool m_bPlayed = { false };

	CGameObject* m_pPlayer = { nullptr };
	Vec3 m_vecMyPos = {};

public:
	static CTriggerBox_EnvJukebox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject* Clone(void* pArg)override;
	virtual	void					Free()override;
};

NS_END