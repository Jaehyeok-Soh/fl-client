#include "Engine_pch.h"
#include "HybridGameObjectBase.h"
#include "EffectBase.h"
#include "GameInstance.h"

CHybridGameObjectBase::CHybridGameObjectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{
}

CHybridGameObjectBase::CHybridGameObjectBase(const CHybridGameObjectBase& rhs)
	:Super(rhs)
{
}

HRESULT CHybridGameObjectBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObjectBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
	{
		MSG_BOX("CHybridGameObjectBase::Initialize, null desc");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHybridGameObjectBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	for (auto* pElement : m_vecModules)
		if (pElement)
			pElement->Awake(iCurrentLevelID);

	return S_OK;
}

void CHybridGameObjectBase::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	Tick_StateModules_Priority(fTimeDelta);
}

void CHybridGameObjectBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Update_HybridState(fTimeDelta);
	Tick_StateModules_Update(fTimeDelta);
}

void CHybridGameObjectBase::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Tick_StateModules_Late(fTimeDelta);
}

void CHybridGameObjectBase::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	Tick_StateModules_BeforeRender(fTimeDelta);
}

HRESULT CHybridGameObjectBase::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Tick_State_Modules_Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObjectBase::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	m_iPrevState = UINT_MAX;
	m_iCurrentState = 0;

	Disable_AllModules();
	return S_OK;
}

HRESULT CHybridGameObjectBase::Despawn_FromPool()
{	
	End_HybridState(m_iCurrentState);
	Disable_StateModules(m_iCurrentState);
	Disable_AllModules();

	m_iPrevState = UINT_MAX;
	m_iCurrentState = 0;

	return Super::Despawn_FromPool();
}

void CHybridGameObjectBase::Change_HybridState(_uint iState)
{
	if (m_iCurrentState == iState)
		return;

	End_HybridState(m_iCurrentState);
	Disable_StateModules(m_iCurrentState);

	m_iPrevState = m_iCurrentState;
	m_iCurrentState = iState;

	Start_HybridState(m_iCurrentState);
	Enable_StateModules(m_iCurrentState);
}

HRESULT CHybridGameObjectBase::Regist_Module(EHybridModuleType eType, CGameObject* pModule)
{
	if (pModule == nullptr)
	{
		MSG_BOX("CHybridGameObjectBase::Regist_Module, module is nullptr");
		return E_FAIL;
	}

	m_vecModules.push_back(pModule);
	Disable_Module(eType, pModule);
	return S_OK;
}

HRESULT CHybridGameObjectBase::Bind_ModuleToState(_uint iState, EHybridModuleType eType, CGameObject* pModule)
{
	if (pModule == nullptr)
	{
		MSG_BOX("CHybridGameObjectBase::Bind_ModuleToState, module is nullptr");
		return E_FAIL;
	}

	m_mapStateModules[iState].emplace_back(std::pair{ eType, pModule });
	return S_OK;
}

HRESULT CHybridGameObjectBase::Add_EffectModule(_uint iPrototypeLevelIndex, const string& strEffectName, const wstring& wstrEffectPrototypeTag, _uint iState)
{
	_uint iHashTag = Engine_Utils::ToHash(strEffectName.c_str());
	auto pData = m_pGameInstance->Find_EffectData(iHashTag);
	if (pData == nullptr)
	{
		MSG_BOX("CHybridGameObjectBase::Add_EffectModule, data is invalid");
		return E_FAIL;
	}

	CBase* pBase =
		m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
		iPrototypeLevelIndex,
		wstrEffectPrototypeTag,
			pData);

	if (pBase == nullptr)
		return E_FAIL;		

	if (FAILED(Regist_Module(EHybridModuleType::EFFECT, static_cast<CGameObject*>(pBase))))
		return E_FAIL;
	if (FAILED(Bind_ModuleToState(iState, EHybridModuleType::EFFECT, static_cast<CGameObject*>(pBase))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObjectBase::Add_CollideModule(_uint iState, PHYSICSCOLLIDER_DESC* colliderDesc, PHYSICSRIGIDBODY_DESC* rigidbodyDesc)
{
	CColliderModule::COLLIDERMODULE_COPY_DESC desc{};
	desc.pPhysicsColliderDesc = colliderDesc;
	desc.pPhysicsRigidbodyDesc = rigidbodyDesc;

	CBase* pBase = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
		/*static*/0,
		L"Prototype_GameObject_ColliderModule",
		&desc);

	if (pBase == nullptr)
		return E_FAIL;

	if (FAILED(Regist_Module(EHybridModuleType::COLLIDER, static_cast<CGameObject*>(pBase))))
		return E_FAIL;
	if (FAILED(Bind_ModuleToState(iState, EHybridModuleType::COLLIDER, static_cast<CGameObject*>(pBase))))
		return E_FAIL;

	return S_OK;
}

void CHybridGameObjectBase::Enable_StateModules(_uint iState)
{
	auto itr = m_mapStateModules.find(iState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		Enable_Module(Pair.first, Pair.second);
		On_ModuleEnter(Pair.first, Pair.second);
	}
}

void CHybridGameObjectBase::Disable_StateModules(_uint iState)
{
	auto itr = m_mapStateModules.find(iState);
	if (itr == m_mapStateModules.end())
		return;

	// Exit ¸ÕÀú ½Ï~
	for (auto& Pair : itr->second)
		On_ModuleExit(Pair.first, Pair.second);
	
	for (auto& Pair : itr->second)
		Disable_Module(Pair.first, Pair.second);
}

void CHybridGameObjectBase::Disable_AllModules()
{
	for (auto* pElement : m_vecModules)
	{
		if (pElement == nullptr)
			continue;

		pElement->Set_CollideEnabled(false);
		pElement->Set_Render(false);
		pElement->Set_Active(false);
	}
}

void CHybridGameObjectBase::Enable_Module(EHybridModuleType eType, CGameObject* pModule)
{
	if (pModule == nullptr)
		return;

	switch (eType)
	{
	case Engine::EHybridModuleType::EFFECT:
	{
		pModule->Set_Active(true);
		pModule->Set_Render(true);
		pModule->Set_CollideEnabled(false);
	} break;
	case Engine::EHybridModuleType::COLLIDER:
	{
		pModule->Set_Active(true);
		pModule->Set_Render(false);
		pModule->Set_CollideEnabled(true);
	} break;
	default:
		pModule->Set_Active(true);
		break;
	}
}

void CHybridGameObjectBase::Disable_Module(EHybridModuleType eType, CGameObject* pModule)
{
	if (pModule == nullptr)
		return;

	pModule->Set_Active(false);
	pModule->Set_Render(false);
	pModule->Set_CollideEnabled(false);
}

void CHybridGameObjectBase::Tick_StateModules_Priority(const _float fTimeDelta)
{
	auto itr = m_mapStateModules.find(m_iCurrentState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		if (Pair.second && Pair.second->Is_Active())
		{
			Pair.second->Update_Priority(fTimeDelta);
		}
	}
}

void CHybridGameObjectBase::Tick_StateModules_Update(const _float fTimeDelta)
{
	auto itr = m_mapStateModules.find(m_iCurrentState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		if (Pair.second && Pair.second->Is_Active())
		{
			Pair.second->Update(fTimeDelta);
		}
	}
}

void CHybridGameObjectBase::Tick_StateModules_Late(const _float fTimeDelta)
{
	auto itr = m_mapStateModules.find(m_iCurrentState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		if (Pair.second && Pair.second->Is_Active())
		{
			Pair.second->Update_Late(fTimeDelta);
		}
	}
}

void CHybridGameObjectBase::Tick_StateModules_BeforeRender(const _float fTimeDelta)
{
	auto itr = m_mapStateModules.find(m_iCurrentState);
	if (itr == m_mapStateModules.end())
		return;

	for (auto& Pair : itr->second)
	{
		if (Pair.second && Pair.second->Can_Render())
		{
			Pair.second->Ready_Before_Render(fTimeDelta);
		}
	}
}

HRESULT CHybridGameObjectBase::Tick_State_Modules_Render()
{
	auto it = m_mapStateModules.find(m_iCurrentState);
	if (it == m_mapStateModules.end())
		return S_OK;

	for (auto& slot : it->second)
	{
		if (FAILED(slot.second->Render()))
			return E_FAIL;
	}			

	return S_OK;
}

void CHybridGameObjectBase::Free()
{
	Disable_AllModules();

	for (auto& element : m_mapStateModules)
		element.second.clear();
	m_mapStateModules.clear();

	for (auto* pElement : m_vecModules)
		if(pElement)
			Safe_Release(pElement);
	m_vecModules.clear();

	Super::Free();
}