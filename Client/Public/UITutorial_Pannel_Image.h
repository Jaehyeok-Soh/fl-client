#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)
class CUITutorial_Pannel_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUITutorialPannelImageDesc : public DIMAGE_DESC
	{
	}TUTORIAL_PANNEL_IMAGE_DESC;
private:
	CUITutorial_Pannel_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUITutorial_Pannel_Image(const CUITutorial_Pannel_Image& rhs);
	virtual ~CUITutorial_Pannel_Image() = default;
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
	HRESULT Ready_Components(TUTORIAL_PANNEL_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
	virtual HRESULT Spawn_FromPool(void* pArg)override;
	virtual HRESULT Despawn_FromPool()override;
private:
	CGameObject* m_pTargetMoster = { nullptr };
	_bool m_isSpawned = { false };

	DelegateHandle m_tEventHandle = {};

	//TUTORIAL_PANNEL_TOP_BG
public:
	static CUITutorial_Pannel_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END