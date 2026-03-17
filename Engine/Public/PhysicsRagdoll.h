#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CComputeShader;

class ENGINE_DLL CPhysicsRagdoll final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::PX_RAGDOLL;

	enum class CS_IDX { IMMU_BONEDATA, MU_MATRIX};

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

	// getter func
public:
	const RAGDOLLELEMENTS& Get_RagDollElements() const { return m_tRagdollElements; }

	// cs func
public:
	HRESULT Bind_RagDollCS_ImmuData(CComputeShader* pRagDollCS);

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