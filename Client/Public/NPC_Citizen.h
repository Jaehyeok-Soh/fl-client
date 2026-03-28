#pragma once
#include "NPC_Base.h"

NS_BEGIN(Client)



/* 임마가 Childe인지 뭔지 아닌지에대해서 업데이트를 할때 알게뭐야. */

class CNPC_Citizen final : public CNPC_Base
{
	using Super = CNPC_Base;
public:

	typedef struct tagNPC_Citizen_Desc : public Super::NPC_DESC
	{
		DTO::CITIZEN_DATA tCitizenData{};
	}NPC_CITIZEN_DESC;
protected:
	CNPC_Citizen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNPC_Citizen(const CNPC_Citizen& rhs);
	virtual ~CNPC_Citizen() = default;

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	HRESULT					Ready_CitizenParts(CNPC_Citizen::NPC_CITIZEN_DESC* pDesc);
	HRESULT					Ready_Component(CNPC_Citizen::NPC_CITIZEN_DESC* pArgs);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID) override;
	virtual void			Update_Priority(const _float fTimeDelta) override;
	virtual void			Update(const _float fTimeDelta) override;
	virtual void			Update_Late(const _float fTimeDelta) override;
	virtual void			Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool			On_Hit(const HIT_DESC& hitDesc) override;
	virtual void			Try_Attack(const HIT_DESC& hitDesc) override;
protected:
	// IQuest을(를) 통해 상속됨
	virtual void			QuestEnter() override;
	virtual void			QuestExit() override;

	// IInteractable을(를) 통해 상속됨
	virtual void			Interact() override;
protected:
	DTO::CITIZEN_DATA		m_tCitizenData;
public:
	static	NPC_DESC		Get_PreSetDesc(_uint iLevelId);
	static  CNPC_Citizen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END

