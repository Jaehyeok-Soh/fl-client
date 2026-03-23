#pragma once
#include "InteractiveObject.h"

NS_BEGIN(Client)

class CChangeLevelObject final : public CInteractiveObject
{
	using Super = Client::CInteractiveObject;

public:
	typedef struct tagInteract_LevelChange_Desc : public CInteractiveObject::INTERACTIVEOBJECT_DESC
	{
		ELevelType				eChangeLevelType{ ELevelType::END };
	}INTERACT_LEVELCHANGE_DESC;

protected:
	CChangeLevelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChangeLevelObject(const CChangeLevelObject& rhs);
	virtual ~CChangeLevelObject() {}

protected:
	virtual HRESULT					Initialize_Prototype()							override;
	virtual HRESULT					Initialize(void* pArg)							override;

public:
	virtual HRESULT					Awake(const _uint iCurrentLevelID)				override;
	virtual void					Update_Priority(const _float fTimeDelta)		override;
	virtual void					Update(const _float fTimeDelta)					override;
	virtual void					Update_Late(const _float fTimeelta)				override;
	virtual void					Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT					Render()										override;

private:
	ELevelType				m_eChangeLevelType{ ELevelType::END };

public:
	static CChangeLevelObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual					void	Free() override;

public:
	// IInteractable을(를) 통해 상속됨
	void Interact() override;

	// IQuest을(를) 통해 상속됨
	void QuestEnter() override;
	void QuestExit() override;
};

NS_END