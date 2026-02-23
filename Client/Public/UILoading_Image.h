#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUILoading_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUILoadingImageDesc : public DIMAGE_DESC
	{
	}LOADING_IMAGE_DESC;

private:
	CUILoading_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUILoading_Image(const CUILoading_Image& rhs);
	virtual ~CUILoading_Image() = default;

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
	HRESULT Ready_Components(LOADING_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;

private:
	_float m_fTimeAcc = {};
	_float m_fDelayTimeAcc = {};

	Vec2 m_vOriginPos = {};
	Vec2 m_vTargetPos = {};

public:
	static CUILoading_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END