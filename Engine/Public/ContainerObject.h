#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
	using Super = CGameObject;
protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CContainerObject(const CContainerObject& rhs);
	virtual ~CContainerObject() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurretLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc);
	virtual HRESULT Render() override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void Set_Active(_bool bActive) override;
	virtual void Set_Render(_bool bRender) override;
	virtual void Set_CollideEnabled(_bool bCollide) override;
	virtual void Set_PartActive(_uint iPartIndex, _bool bActive);
	virtual void Set_PartRender(_uint iPartIndex, _bool bRender);
	virtual void Set_PartCollideEnabled(_uint iPartIndex, _bool bCollide);
public:
	template<typename T>
	T* Get_Part(_uint iPartID);
	
	void Remove_Part(_uint iPartID);
	HRESULT Add_Part(class CPartObject* pPart, _uint iPartID);
	HRESULT Add_Part(_uint iPartID, _uint iPrototypeLevelIndex,const  wstring &wstrPrototypeTag, void* pArg);
	HRESULT Change_Part(class CPartObject* pPart, _uint iPartID);
	HRESULT Change_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg);
	_uint Get_PartSize() { return (_uint)m_vecPartObjects.size(); }
protected:
	vector<class CPartObject*> m_vecPartObjects;
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

template<typename T>
T* CContainerObject::Get_Part(_uint iPartID)
{
	if(!m_vecPartObjects[iPartID])
		return nullptr;

	return static_cast<T*>(m_vecPartObjects[iPartID]);
}

NS_END