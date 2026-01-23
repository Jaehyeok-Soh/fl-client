#pragma once
#include "Base.h"
#include "Transform.h"
#include "MonoBehaviour.h"

NS_BEGIN(Engine)

class CCollider;
class CCameraMan;

class ENGINE_DLL CGameObject abstract : public CBase
{
	using Super = CBase;
public:
	typedef struct tagGameObjectDesc : public CComponent::tagComponentDesc
	{
		_uint iLevelIndex = { 999 };
	}GAMEOBJECT_DESC;
protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	// 객체 생성이 완료되고 Change_Level이 완료되었을 때 호출
	virtual HRESULT Awake(const _uint iCurrentLevelID) PURE;
	virtual void Update_Priority(const _float fTimeDelta);
	virtual void Update(const _float fTimeDelta);
	virtual void Update_Late(const _float fTimeDelta);
	virtual void Ready_Before_Render(const _float fTimeDelta);
	virtual void OnCollision(_uint iMyColliderLayer, CCollider* pOther) {}
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther) {}
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther) {}
	virtual _bool Picking(OUT Vec3& vOut) { return false; }
	virtual HRESULT Render();
	virtual _bool On_Hit(_uint iCollideMyLayer, ATTACK_DESC* pDesc, CGameObject* pOther) { return true; }
	virtual void Try_AttackHit() {};
	template<typename T>
	inline void Change_Component(T* pSrc);
	template<typename T>
	inline T* Get_Component();
	template<typename T>
	HRESULT Add_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg);
	template<typename T>
	HRESULT Add_Component(T* pOriginInstance);
	template<typename T>
	void Remove_Component();
	inline CMonoBehaviour* Get_Script_Component(const wstring& wstrComponentTag);
	inline CMonoBehaviour* Detach_Script_Component(const wstring& wstrComponentTag);
	HRESULT Add_Script_Component(const wstring& wstrComponentTag, CMonoBehaviour* pComp);
	HRESULT Add_Script_Component(const wstring& wstrComponentTag, const wstring& wstrPrototypeTag, void* pArg);
	void Remove_Script_Component(const wstring& wstrComponentTag);
	virtual _int Get_AnimationIndex(const wstring& wstrName);
	virtual HRESULT Change_State(_uint iIndex);
	CCameraMan* Get_CameraTargeter() { return m_pTargeter; }
	void Set_CameraTargeter(CCameraMan* pTargeter) { m_pTargeter = pTargeter; }
	_bool IsClone() const { return m_bClone; }
	
	_bool IsDead() const { return m_bDead; }
	virtual void Set_Dead(const wstring &wstrLayerTag);
	Vec3 Get_CenterFromCollider(EColliderType eType, class CBounding* pBounding);
protected:
	void Update_Script_Components(const _float fTimeDelta);
private:
	void Safe_Release_Component();
	void Safe_Release_ScriptComponent();
protected:
	_bool m_bDead = { false };
	CCameraMan* m_pTargeter = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance *m_pGameInstance = { nullptr };
private:
	_bool m_bClone = { false };
	array<CComponent*, g_ComponentTypeCount> m_Components;
	unordered_map<wstring, CMonoBehaviour*> m_ScriptComponents;
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

template <typename T>
inline T* CGameObject::Get_Component()
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");
	return static_cast<T*>(m_Components[ENUM_TO_UINT(T::_ID)]);
}

template <typename T>
inline HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg)
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");

	if (m_Components[ENUM_TO_UINT(T::_ID)])
		Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);

	if (!((m_Components[ENUM_TO_UINT(T::_ID)] =
		dynamic_cast<CComponent*>(CGameInstance::GetInstance()->Clone_Prototype(EPrototypeType::COMPONENT, iLevelIndex, wstrPrototypeTag, pArg)))))
		return E_FAIL;

	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(this);
	return S_OK;
}

template <typename T>
inline HRESULT CGameObject::Add_Component(T* pOriginInstance)
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");

	if (!pOriginInstance)
		return E_FAIL;

	if (m_Components[ENUM_TO_UINT(T::_ID)])
		Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);

	if (!((m_Components[ENUM_TO_UINT(T::_ID)] =
		dynamic_cast<CComponent*>(pOriginInstance))))
		return E_FAIL;

	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(this);
	return S_OK;
}

template <typename T>
inline void CGameObject::Change_Component(T* pSrc)
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");

	if (m_Components[ENUM_TO_UINT(T::_ID)])
		Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);

	m_Components[ENUM_TO_UINT(T::_ID)] = pSrc;
	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(this);
}

template <typename T>
inline void CGameObject::Remove_Component()
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");
	Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);
	m_Components[ENUM_TO_UINT(T::_ID)] = nullptr;
}
NS_END