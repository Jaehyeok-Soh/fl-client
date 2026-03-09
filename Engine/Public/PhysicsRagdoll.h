#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsRagdoll final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::PX_RAGDOLL;

private:
	CPhysicsRagdoll();
	CPhysicsRagdoll(const CPhysicsRagdoll& rhs);
	virtual ~CPhysicsRagdoll() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake(vector<class CChannel*>& vecChannels);
	void Update();
	void Sleep();

public:
	void SetRagdollLive(_bool bVal);

#ifdef _DEBUG
	void Render();
#endif // _DEBUG

private:
	void SetUserData(CGameObject* pObject);
	void CombinedJoint(RAGDOLLJOINT::Enum eJoint, PxTransform ObjectWorldTransform, PxTransform parentTransform, vector<CChannel*>& vecChannels);

private:
	RAGDOLLELEMENTS m_tRagdollElements = {};
	vector<PxRigidActor*> m_pActors = { nullptr };
	class CModel* m_pSharedModel = { nullptr };
	uint64 m_iObjectID = {};

public:
	static CPhysicsRagdoll* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END