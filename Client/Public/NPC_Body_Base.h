#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class CBone;
NS_END

NS_BEGIN(Client)

class CNPC_Body_Base abstract : public CPartObject
{
	using Super = CPartObject;

public:
	enum class EBone : _uint
	{
		RightHand = 0,
		END
	};
	typedef struct tagNPCBodyDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring wstrModelPrototypeTag = { L"" };
		std::span<std::pair<_uint, string>> spanBoneNames;
	}NPCBODY_DESC;

protected:
	CNPC_Body_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNPC_Body_Base(const CNPC_Body_Base& rhs);
	virtual ~CNPC_Body_Base() = default;

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
	CBone* Get_Bone(CNPC_Body_Base::EBone eBone);
	const Matrix* Get_SocketMatrix(const _char* szBoneName);
	const Matrix* Get_SocketMatrix(_uint iIndex);

protected:
	HRESULT Ready_Components(NPCBODY_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShader();
	HRESULT Ready_Bones(NPCBODY_DESC* pDesc);

protected:
	void Setting_FaceMix(_uint iFaceBoneIdx, _int iAnimIdx);
	void Change_FaceAnim(_int iAnimIdx);

protected:
	std::vector<_uint> m_vecBoneIndices;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END