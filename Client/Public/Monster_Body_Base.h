#pragma once
#include "ICameraAnchorProvider.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CBone;
class CComputeShader;
NS_END

NS_BEGIN(Client)

class CMonster_Body_Base abstract : public CPartObject
								  , public ICameraAnchorProvider
{
	using Super = CPartObject;
public:
	enum class EBone : _uint
	{
		RightHand = 0,
		END
	};
	typedef struct tagMonsterBodyDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring wstrModelPrototypeTag = { L"" };
		std::span<std::pair<_uint, string>> spanBoneNames;
	}MONSTERBODY_DESC;
protected:
	CMonster_Body_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster_Body_Base(const CMonster_Body_Base& rhs);
	virtual ~CMonster_Body_Base() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
public:
	CBone* Get_Bone(CMonster_Body_Base::EBone eBone);
	const Matrix* Get_SocketMatrix(const _char* szBoneName);
	const Matrix* Get_SocketMatrix(_uint iIndex);
public:
	// Camera Interface
	virtual _bool Resolve_CameraAnchor(
		Engine::ECameraAnchorResolve eResolve,
		const string& strAnchorTag, const Matrix& matOwnerWorld,
		OUT Engine::CAMERA_ANCHOR_RESULT& outResult) override;
private:
	CBone* Find_CameraAnchorBone(ECameraAnchorResolve eResolve, const string& strAnchorTag);
protected:
	HRESULT Ready_Components(MONSTERBODY_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShader();
	HRESULT Ready_Bones(MONSTERBODY_DESC* pDesc);
protected:
	std::vector<_uint> m_vecBoneIndices;
	CComputeShader* m_pBoneCombineCS{ nullptr };
	CComputeShader* m_pBoneMeshCS{ nullptr };
	CComputeShader* m_pBoneAnimEvaluateCS{ nullptr };
	CComputeShader* m_pBoneAnimBlendCS{ nullptr };
	CComputeShader* m_pBoneAnimMixCS{ nullptr };
	CComputeShader* m_pRagDollCS{ nullptr };

protected:
	_bool m_bRagDollOnPre = { false };
	_bool m_bRagDollOn = { false };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END