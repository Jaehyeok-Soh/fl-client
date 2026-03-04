#pragma once
#include "Base.h"
#include "Transform.h"
#include "MonoBehaviour.h"
#include "DataEnum.h"

NS_BEGIN(Engine)

enum class ELifeState : _ushort
{
	Alive = 0,
	Dying,
	Pending,
	Pooled,
};

class CCollider;
class CCameraMan;
class CObjectPool;
class CDataDocumentBase;

class ENGINE_DLL CGameObject abstract : public CBase
{
	using Super = CBase;
	static uint64 s_iNextID;
public:
	typedef struct tagGameObjectDesc : public CComponent::tagComponentDesc
	{
		_uint iLevelIndex = { 999 };
	}GAMEOBJECT_DESC;
	typedef struct tagGameObjectReinitDesc
	{

	}GAMEOBJECT_REINIT_DESC;
	enum EFlag : _uint
	{
		NONE = 0,
		ACTIVE = 1 << 0,
		RENDER = 1 << 1,
		COLLIDE = 1 << 2,
		//=================================//
		DEFAULT = ACTIVE | RENDER | COLLIDE,
		ALL = 0xFFFFFFFF,
	};
protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	// Static Object가 다른 Level에 init 될때 호출
	virtual HRESULT Reinitialize(GAMEOBJECT_REINIT_DESC* pDesc) { return S_OK; }
	// Static Object가 다른 Level에 가기전 해당 Level이 정리 될때 호출
	virtual HRESULT Clear_WhenChangeLevel() { return S_OK; }
	// 객체 생성이 완료되고 Change_Level이 완료되었을 때 호출
	virtual HRESULT Awake(const _uint iCurrentLevelID) PURE;
	virtual void Update_Priority(const _float fTimeDelta);
	virtual void Update(const _float fTimeDelta);
	virtual void Update_Late(const _float fTimeDelta);
	virtual void Ready_Before_Render(const _float fTimeDelta);
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) {}
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO &tHitInfo) {}
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) {}
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) {}
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) {}
	virtual _bool Picking(OUT Vec3& vOut) { return false; }
	virtual HRESULT Render();
	inline virtual HRESULT Spawn_FromPool(void* pArg);
	inline virtual HRESULT Despawn_FromPool();
	virtual _bool IntersectWithFrustrum(BoundingFrustum* pFrustrum) { return true; }
	virtual _bool On_Hit(const HIT_DESC& hitDesc) { return true; }
	virtual void Try_Attack(const HIT_DESC& hitDesc) {};
	/* Static Object가 LevelExit할때 Caching 정보 초기화용 */
	virtual void On_LevelExit(_uint iLevelID) {};
	template<typename T>
	inline void Change_Component(T* pSrc);
	template<typename T>
	inline T* Get_Component();
	template<typename T>
	inline HRESULT Add_Component(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, void* pArg);
	template<typename T>
	inline HRESULT Add_Component(T* pOriginInstance);
	template<typename T>
	inline T* Detach_Component();
	template<typename T>
	void Remove_Component();
	inline CMonoBehaviour* Get_Script_Component(const wstring& wstrComponentTag);
	inline CMonoBehaviour* Detach_Script_Component(const wstring& wstrComponentTag);
	HRESULT Add_Script_Component(const wstring& wstrComponentTag, CMonoBehaviour* pComp);
	HRESULT Add_Script_Component(const wstring& wstrComponentTag, const wstring& wstrPrototypeTag, void* pArg, void** ppCaching = nullptr);
	void Remove_Script_Component(const wstring& wstrComponentTag);
	HRESULT Change_Script_Component(const wstring& wsrtTargetComponentTag, CMonoBehaviour* pComp);
	virtual _int Get_AnimationIndex(const wstring& wstrName);
	virtual _int Get_WeaponAnimationIndex(const wstring& wstrName) { return -1; };
	virtual HRESULT Change_State(_uint iIndex);
	CCameraMan* Get_CameraTargeter() { return m_pTargeter; }
	void Set_CameraTargeter(CCameraMan* pTargeter) { m_pTargeter = pTargeter; }
	virtual void Set_Dead(const wstring& wstrLayerTag);
	void Set_ActiveIndex(_uint iActiveIndex) { m_iActiveIndex = (_int)iActiveIndex; }
	_int Get_ActiveIndex() const { return m_iActiveIndex; }
	void Set_Awake(_bool bAwaked) { m_bAwaked = bAwaked; }
	void Set_RenderInfoFlag(_uint iFlag, _bool bActive);
	virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) { return false; }
public:
	void Set_Name(const string& strName);
	void Set_Name(const wstring& wstrName);
	string Get_Name();
	wstring Get_WName();
	wstring Get_Layer() const { return m_wstrLayerTag; }
	void Set_Layer(const wstring& wstrLayerTag) { m_wstrLayerTag = wstrLayerTag; }
	uint64 Get_ID() { return m_iObjectID; }
	void Clear_Components_WhenChangeLevel();
public:
	void Mark_Pooled();
	void Set_Active(_bool b);
	void Set_Render(_bool b);
	void Set_CollideEnabled(_bool b);
public:
	_bool Is_Alive() const { return m_eState == ELifeState::Alive; }
	_bool Is_Dying() const { return m_eState == ELifeState::Dying; }
	_bool Is_Destroy() const { return m_eState == ELifeState::Pending; }
	_bool Is_Pooled() const { return m_eState == ELifeState::Pooled; }

	inline _bool Is_Active() const;
	inline _bool Can_Collide() const;
	inline _bool Can_Render() const;

	inline void Set_Dying();
	inline void Set_Destroy(_bool bIsStatic = false);

	_bool Is_Awaked() const { return m_bAwaked; }
	_bool IsClone() const { return m_bClone; }
private:
	void Clamp_FlagsByState();
	void Disable_CollisionComponent();
	void Update_Script_Components(const _float fTimeDelta);
	void Safe_Release_Component();
	void Safe_Release_ScriptComponent();
protected:
	_int m_iActiveIndex = { -1 };
	wstring m_wstrLayerTag = { L"" };
	CCameraMan* m_pTargeter = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance *m_pGameInstance = { nullptr };
	uint64 m_iObjectID = { 0 };
	string m_strName = { "" };
	SHADER_OBJECTINFO_DESC m_tObjectInfoDesc{};
private:
	_bool m_bAwaked = { false };
	ELifeState m_eState = { ELifeState::Alive };
	CObjectPool* m_pOwnerPool = { nullptr };
	_uint m_iObjectFlags = { CGameObject::EFlag::DEFAULT };
	_bool m_bClone = { false };
	array<CComponent*, g_ComponentTypeCount> m_Components;
	unordered_map<wstring, CMonoBehaviour*> m_ScriptComponents;
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
	friend class CObject_Manager;
};

/// <summary>
/// <para>컴포넌트를 참조하는 함수</para>
/// <para>타입캐스팅이 이루어져서 가져옴</para>
/// RefCount책임은 외부에
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template <typename T>
inline T* CGameObject::Get_Component()
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");
	return static_cast<T*>(m_Components[ENUM_TO_UINT(T::_ID)]);
}

/// <summary>
/// <para>컴포넌트를 추가하는 함수</para>
/// 내부적으로 원본을 복사 생성하여 추가
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="iPrototypeLevelIndex">원본의 LevelID</param>
/// <param name="wstrPrototypeTag">원본의 태그</param>
/// <param name="pArg">객체 Desc</param>
/// <returns></returns>
template <typename T>
inline HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, void* pArg)
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");

	if (m_Components[ENUM_TO_UINT(T::_ID)])
		Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);

	if (!((m_Components[ENUM_TO_UINT(T::_ID)] =
		dynamic_cast<CComponent*>(CGameInstance::GetInstance()->Clone_Prototype(EPrototypeType::COMPONENT, iPrototypeLevelIndex, wstrPrototypeTag, pArg)))))
		return E_FAIL;

	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(this);
	return S_OK;
}

/// <summary>
/// <para>컴포넌트를 추가하는 함수</para>
/// 파라미터로 인스턴스를 받아 추가하며, 해당 슬롯에 컴포넌트가 추가되어있었다면 Safe_Release 처리
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="pComp"></param>
/// <returns></returns>
template <typename T>
inline HRESULT CGameObject::Add_Component(T* pComp)
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");

	if (!pComp)
		return E_FAIL;

	if (m_Components[ENUM_TO_UINT(T::_ID)])
		Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);

	if (!((m_Components[ENUM_TO_UINT(T::_ID)] =
		dynamic_cast<CComponent*>(pComp))))
		return E_FAIL;

	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(this);
	return S_OK;
}

/// <summary>
/// <para>컴포넌트를 탈착시키는 함수</para>
/// <para>Remove_Component와 달리 Safe_Release하지 않고 슬롯에서 인스턴스를 빼옴</para>
/// <para>이때, Owner는 자동적으로 nullptr 처리</para>
/// 해당 Component의 생명주기는 호출자에게 달려있음
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template<typename T>
inline T* CGameObject::Detach_Component()
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");
	
	if (m_Components[ENUM_TO_UINT(T::_ID)] == nullptr)
		return nullptr;

	m_Components[ENUM_TO_UINT(T::_ID)]->Set_Owner(nullptr);
	T* Return{ nullptr };
	Return = m_Components[ENUM_TO_UINT(T::_ID)];
	m_Components[ENUM_TO_UINT(T::_ID)] = nullptr;
	return Return;
}

/// <summary>
/// <para>컴포넌트를 교체하는 함수</para>
/// 기존 슬롯에 있던 컴포넌트는 Safe_Release처리
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="pSrc"></param>
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

/// <summary>
/// 컴포넌트를 삭제하는 함수
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
inline void CGameObject::Remove_Component()
{
	static_assert(T::_ID >= EComponentType::TRANSFORM || T::_ID < EComponentType::END, "ComponentType ID is invalid");
	static_assert(T::_ID != EComponentType::SCRIPT, "Script type components dont support this feature. Please check the function name");
	Safe_Release(m_Components[ENUM_TO_UINT(T::_ID)]);
	m_Components[ENUM_TO_UINT(T::_ID)] = nullptr;
}
NS_END