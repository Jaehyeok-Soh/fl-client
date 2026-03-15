#pragma once
#include "GameObject.h"
#include "IQuest.h"

NS_BEGIN(DTO)

struct SRT_DATA;

NS_END

NS_BEGIN(Client)

class CTriggerBox abstract : public CGameObject, public IQuest
{
public:
	enum class Type
	{
		CHANGE_LEVEL,
		MONSTER_SPAWNER,
		GLOBALEVENT_BROADCASTER,
		END,
	};

	using Super = CGameObject;
protected:
	typedef struct tagTriggerBox_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		/* Trigger Box */
		Vec3					vTriggerBox_Extents{1.f,1.f,1.f};
		/* ÇÊ¼ö */
		const DTO::SRT_DATA*	pSRTData{nullptr};

		_bool					bHasQuest = { false };
		DTO::QUEST_CHAPTERDESC	tQuestObjectDesc = {};
	}TRIGGERBOX_DESC;
protected:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_DESC* pDesc);
	void					Ready_Quest(DTO::QUEST_CHAPTERDESC* pQuestDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	CTriggerBox::Type		Get_TriggerBoxType() const { return m_eTriggerBoxType; }

	virtual void			QuestEnter() override {};
	virtual void			QuestExit() override {};

protected:
	_bool					m_isTriggerEventPlay{false};
	CTriggerBox::Type		m_eTriggerBoxType{ CTriggerBox::Type::END};

public:
	virtual					void Free()override;
};

NS_END

