#pragma once
#include "ContainerObject.h"
#include "IQuest.h"
#include "IInteractable.h"
#include "DataStruct_Map.h"

NS_BEGIN(Engine)
class CEffectHandler;
NS_END

NS_BEGIN(Client)

class CNPC_Base abstract :
	public CContainerObject,
	public IQuest,
	public IInteractable
{
	using Super = CContainerObject;

public:
	typedef struct tagNPCDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = {};
		wstring wstrPartBodyPrototypeTag = {};
		wstring wstrNavigationPrototypeTag = {};

		_int	iNavigationCellIndex = { -1 };
		Vec3	vSpawnPosition = {};
		wstring wstrNPCStateTag = {};

		std::span<std::pair<_uint, string>>		spanBoneNames;
		PHYSICSCCT_DESC tCCTDesc{};
		_bool bHasQuest = { false };
		vector<DTO::QUEST_CHAPTERDESC>			tQuestObjectDesc = {};
	}NPC_DESC;

	struct Part
	{
		enum Enum : _uint
		{
			BODY = 0,
			SWORD,
			GUN,
			END
		};
	};

protected:
	CNPC_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNPC_Base(const CNPC_Base& rhs);
	virtual ~CNPC_Base() = default;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT		Awake(const _uint iCurrentLevelID) override;
	virtual void		Update_Priority(const _float fTimeDelta) override;
	virtual void		Update(const _float fTimeDelta) override;
	virtual void		Update_Late(const _float fTimeDelta) override;
	virtual void		Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual _int		Get_AnimationIndex(const wstring& wstrName) override;
	virtual _wstring	Get_AnimationName(_uint iAniIndex);

public:
	virtual void		OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void		OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void		OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void		OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void		OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool		On_Hit(const HIT_DESC& hitDesc) override;
	virtual void		Try_Attack(const HIT_DESC& hitDesc) override;

	void				Set_RootMotion_Apply(_bool bApply);

protected:

	HRESULT				Ready_BaseStates();
	HRESULT				Ready_PartObjects(void* pArg);
	HRESULT				Ready_Components(void* pArgs);
	HRESULT				Ready_EffectHandler(void* pArgs);
	HRESULT				Ready_CCT(void* pArgs);
	void				Ready_Quest(vector<DTO::QUEST_CHAPTERDESC>* pQuestDesc);

protected:
	
	// IQuest을(를) 통해 상속됨
	virtual void QuestEnter()override;
	virtual void QuestExit()override;

	// IInteractable을(를) 통해 상속됨
	virtual void Interact()override;
private:
	CEffectHandler* m_pEffectHandler = {nullptr};

public:
	static  HRESULT			Create_NPC(BATCH_NPC_DESC* pDesc, _uint iFindPrototypeLevelType, _uint iAddLevelType, CTransform::TRANSFORM_DESC* pTransformDesc = nullptr);
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END