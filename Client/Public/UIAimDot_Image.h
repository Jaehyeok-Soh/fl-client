#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;

class CUIAimDot_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIAimDotImageDesc : public DIMAGE_DESC
	{
	}AIMDOT_IMAGE_DESC;

private:
	CUIAimDot_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIAimDot_Image(const CUIAimDot_Image& rhs);
	virtual ~CUIAimDot_Image() = default;
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
	HRESULT Ready_Components(AIMDOT_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };
	_bool m_isHitScan = { false };

	_bool m_isPreRangeAtt = { false };
	_bool m_isPreMeeleAtt = { false };

	_float m_fAttSpeed = {0.1f};
	_bool m_isShootingTrigger = {};
	Vec2 m_vMaxOffset = {};

	_bool m_isSpreadStart = {};
	_bool m_isSpreadEnd = {};

public:
	static CUIAimDot_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END