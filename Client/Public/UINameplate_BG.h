#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Client)
class CUINameplate_BG final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagNamePlateBGDesc : public DIMAGE_DESC
	{
	}NAMEPLATE_BG_DESC;
private:
	CUINameplate_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUINameplate_BG(const CUINameplate_BG& rhs);
	virtual ~CUINameplate_BG() = default;
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
	HRESULT Ready_Components(NAMEPLATE_BG_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;
private:
	CGameObject* m_pTargetMoster = { nullptr };
	_bool m_isSpawned = { false };

public:
	static CUINameplate_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END