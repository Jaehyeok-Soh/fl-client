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

	_int FindRagdollJointByBoneIndex(_uint boneIdx);

#ifdef _DEBUG
	void Render();
#endif // _DEBUG

private:
	void CombinedJoint(RAGDOLLJOINT::Enum eJoint, PxTransform ObjectWorldTransform, PxTransform parentTransform, vector<CChannel*>& vecChannels, vector<class CBone*>& vecBone);
	PxTransform BoneCombine(class CBone* pCurrentBone, PxTransform pxLocal, class CBone* pParentBone, vector<CChannel*>& vecChannels, vector<class CBone*>& vecBone);

private:
	RAGDOLLELEMENTS m_tRagdollElements = {};
	class CModel* m_pSharedModel = { nullptr };
	uint64 m_iObjectID = {};

public:
	static CPhysicsRagdoll* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END