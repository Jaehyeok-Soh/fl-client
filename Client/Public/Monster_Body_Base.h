#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class CBone;
class CEffectHandler;
NS_END

NS_BEGIN(Client)

class CMonster_Body_Base abstract : public CPartObject
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
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;
public:
	CBone* Get_Bone(CMonster_Body_Base::EBone eBone);
	const Matrix* Get_SocketMatrix(const _char* szBoneName);
	const Matrix* Get_SocketMatrix(_uint iIndex);

protected:
	HRESULT Ready_Components(MONSTERBODY_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShader();
	HRESULT Ready_Bones(MONSTERBODY_DESC* pDesc);
	HRESULT Ready_EffectHandler(MONSTERBODY_DESC* pDesc);
protected:
	std::vector<_uint> m_vecBoneIndices;
	class CEffectHandler* m_pEffectHandler = { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END