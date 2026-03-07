#include "Engine_pch.h"
#include "HybridGameObject_Base.h"
#include "EffectBase.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

CHybridGameObject_Base::CHybridGameObject_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EHyBridEffectType eType)
	:Super(pDevice, pDeviceContext)
	,eHybridType(eType)
{

}

CHybridGameObject_Base::CHybridGameObject_Base(const CHybridGameObject_Base& rhs)
	:Super(rhs)
	,eHybridType(rhs.eHybridType)
{

}

HRESULT CHybridGameObject_Base::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObject_Base::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_EffectModule(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObject_Base::Ready_EffectModule(void* pArg)
{
	Origin_HybridDesc* pDesc = static_cast<Origin_HybridDesc*>(pArg);
	
	if (pDesc == nullptr)
	{
		MSG_BOX("EffectModule Create Fail : CHybridGameObject_Base");
		return E_FAIL;
	}

	auto& ModuleList = pDesc->m_ModuleEffect;
	wstring PrototypeTag = {};
	PrototypeTag = L"Prototype_GameObject_Effect_WarningCircle";

	for (_uint i = 0; i < (_uint)ModuleList.size(); ++i)
	{
		_uint iHashTag = ModuleList[i].second;
		void* pEffectData = m_pGameInstance->Find_EffectData(iHashTag);
		
		CBase* pBase = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			/*Static*/0,
			PrototypeTag,
			pEffectData);

		if (pBase == nullptr)
			continue;

		m_StateModules[ModuleList[i].first].push_back(
			std::make_pair(EHybridModuleType::EFFECT, static_cast<CGameObject*>(pBase))
		);
	}

	return S_OK;
}


HRESULT CHybridGameObject_Base::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CHybridGameObject_Base::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);

	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if (pModule.second)
				pModule.second->Update_Priority(fTimeDelta);
		}
	}

	// 업데이트가 돌기전에 Check를 한번 한다.
	Update_CheckState();
}

void CHybridGameObject_Base::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if(pModule.second)
				pModule.second->Update(fTimeDelta);
		}
	}
}

void CHybridGameObject_Base::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if (pModule.second)
				pModule.second->Update_Late(fTimeDelta);
		}
	}
}

void CHybridGameObject_Base::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if (pModule.second)
				pModule.second->Ready_Before_Render(fTimeDelta);
		}
	}
}

HRESULT CHybridGameObject_Base::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if (pModule.second)
				pModule.second->Render();
		}
	}

	return S_OK;
}

HRESULT CHybridGameObject_Base::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybridGameObject_Base::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	return S_OK;
}

void CHybridGameObject_Base::Change_HybridState(_uint iState)
{

}

void CHybridGameObject_Base::Free()
{
	for (auto& Module : m_StateModules)
	{
		for (auto& Object : Module.second)
		{
			if (Object.second)
				Safe_Release(Object.second);
		}
	}

	Super::Free();
}