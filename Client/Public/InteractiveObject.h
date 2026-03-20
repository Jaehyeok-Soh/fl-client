#pragma once
#include "GameObject.h"
#include "IInteractable.h"
#include "IQuest.h"


NS_BEGIN(Engine)
struct BATCH_INTERACTIVEOBJECT_DESC;
NS_END

NS_BEGIN(Client)

class CInteractiveObject abstract: public CGameObject, public IInteractable , public IQuest
{
	using Super = Engine::CGameObject;
public:
	typedef struct InteractiveObject_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint						   flagFilterLayer = PHYSICSFILTERGROUP::OBJECT1;
		_uint						   flagFilterMaks = PHYSICSFILTERGROUP::DETECT_INTERACT | PHYSICSFILTERGROUP::PLAYER;

		EModelType					   eModelType{EModelType::STATIC};
		wstring						   wstrModelPath{L""};
		vector<DTO::QUEST_CHAPTERDESC> vecQuestDesc{}; /* Quest 관련 정보 없으면 Empty */
	}INTERACTIVEOBJECT_DESC;
protected:
	CInteractiveObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInteractiveObject(const CInteractiveObject& rhs);
	virtual ~CInteractiveObject() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(INTERACTIVEOBJECT_DESC* pDesc);
	HRESULT					Ready_Quest(const vector<DTO::QUEST_CHAPTERDESC>& vecQusetDesc);

	HRESULT					Ready_PhysicsCollider(INTERACTIVEOBJECT_DESC* pDesc);
	HRESULT					Ready_PhysicsRigidBody(INTERACTIVEOBJECT_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)override;
public:
	virtual					void	Free() override;
public:
	static	HRESULT			Create_InteractiveObject(const BATCH_INTERACTIVEOBJECT_DESC* pToolDesc , _uint iAddLevelIndex , const wstring& wstrModelPath , CTransform::TRANSFORM_DESC* pTsDesc = nullptr);

	// IInteractable을(를) 통해 상속됨
	void					Interact() override;

	// IQuest을(를) 통해 상속됨
	void					QuestEnter() override;
	void					QuestExit() override;
};

NS_END