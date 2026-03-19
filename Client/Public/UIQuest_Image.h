#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)
class CUIQuest_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIQuestImageDesc : public DIMAGE_DESC
	{
	}QUEST_IMAGE_DESC;

private:
	CUIQuest_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIQuest_Image(const CUIQuest_Image& rhs);
	virtual ~CUIQuest_Image() = default;
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
	HRESULT Ready_Components(QUEST_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	// 퀘스트가 보여지고 있었는가
	_bool m_isChanged = {false};
	_bool m_isInitialized = { false };
	_bool m_isMovedIn = { false };
	_bool m_isMovedOut = { false };

	_bool m_isVisibleTrigger = { false };
	_bool m_isVisibleTriggerStart = { false };

public:
	static CUIQuest_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END