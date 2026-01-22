#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagPartObjectDesc : public Super::GAMEOBJECT_DESC
	{
		const _float4x4* pMatParent = { nullptr };
	}PARTOBJ_DESC;
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollision(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther) override;
public:
	virtual void Set_AttackCollider(_bool bActive, ATTACK_DESC* pDesc);
	ATTACK_DESC* Get_AttackDesc() { return &m_CurrentAttackDesc; }

	_bool Is_AttackWindow() const { return m_bAttackWindow; }
	void Begin_AttackWindow(ATTACK_DESC* pDesc);
	void End_AttackWindow();
	void Clear_HitTargets() { m_unsetHitTargets.clear(); }
	_bool Is_AlreadyHitted(CGameObject* pVictim) const { return (pVictim != nullptr) && (m_unsetHitTargets.find(pVictim) != m_unsetHitTargets.end()); }
	void Regist_HitTargets(CGameObject* pVictim) { if(pVictim) m_unsetHitTargets.insert(pVictim); }

	void Set_Parent(CGameObject* pGo);
	CGameObject* Get_Parent() { return m_pParentObject; }
protected:
	void Clear_AttackDesc();
protected:
	void Update_CombinedWorldMatrix(const _float4x4* pMatParent);
	void Update_CombinedWorldMatrix(_fmatrix matParent);
	void Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent);
	void Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent, _float2 vUIScale);
protected:
	_bool m_bAttackWindow = { false };
	CGameObject* m_pParentObject = { nullptr };
	const _float4x4* m_pMatParent = { nullptr };
	_float4x4 m_matCombinedWorld = {};
	ATTACK_DESC m_CurrentAttackDesc = {};
	unordered_set<CGameObject*> m_unsetHitTargets;
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END