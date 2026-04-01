#pragma once
#include "NPC_Base.h"

NS_BEGIN(Client)






/* 임마가 Childe인지 뭔지 아닌지에대해서 업데이트를 할때 알게뭐야. */

class CNPC_Citizen final : public CNPC_Base
{
	using Super = CNPC_Base;
public:
	enum class Part
	{
		Body = 0,
		Hair = 1,
		Beard = 2,
		END,
	};
public:
	typedef struct tagNPC_Citizen_Desc : public Super::NPC_DESC
	{
		_bool								isWalking{ false }; /* WayPoint 정보대로 걸어다니는 애들인가 아닌가 이게 _bool 인애들은 Preset Data에서 Random으로 옷을 갈아껴입어준다 */
		DTO::CITIZEN_DATA					tCitizenData{};
	}NPC_CITIZEN_DESC;	

	typedef struct tagNPC_Citizen_Pool_Desc
	{
		DTO::Citizen_MoveData tMoveData{};
	}NPC_CITIZEN_POOL_DESC;

protected:
	CNPC_Citizen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNPC_Citizen(const CNPC_Citizen& rhs);
	virtual ~CNPC_Citizen() = default;
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Spawn_FromPool(void* pArg) override;
	HRESULT					Ready_CitizenParts(CNPC_Citizen::NPC_CITIZEN_DESC* pDesc);
	HRESULT					Ready_Component(CNPC_Citizen::NPC_CITIZEN_DESC* pArgs);
	HRESULT					Change_WalkCitizenModel(const DTO::CITIZEN_DATA& tData);

	virtual HRESULT			Setting_NPCText(_uint iCurrentLevelID)override;
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID) override;
	virtual void			Update_Priority(const _float fTimeDelta) override;
	virtual void			Update(const _float fTimeDelta) override;
	virtual void			Update_Late(const _float fTimeDelta) override;
	virtual void			Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT			Render() override;
public:

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
private:
	void					Reset_WayPoint();
	void					Reset_MoveData();
public:
	_bool					Get_IsArrive() const { return m_isArrive; }
protected:
	_bool								m_isWalking;
	DTO::CITIZEN_DATA					m_tCitizenData;




	/* WayPoint 전용 */
	_bool								m_isOnReachWayPointtDissloveStart;
	_bool								m_isArrive;
	DTO::CitizenWalkRunAnimIndex		m_tWalkRunAnimIndex;
	float								m_fDeltaTime;
	const DTO::Citizen_WayPoint_Data*	m_pWayPointData;
	_uint								m_iCurrentFrameIndex;
	Vec3								m_vLastPosition;
	float								m_fDisloveRange;

public:

public:
	static	NPC_DESC		Get_PreSetDesc(_uint iLevelId);
	static  CNPC_Citizen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END

