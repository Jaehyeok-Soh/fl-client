#include "pch.h"
#include "Hybrid_WarningSpace.h"
#include "Effect_WarningCircle.h"
#include "GameInstance.h"

CHybrid_WarningSpace::CHybrid_WarningSpace(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext, EHyBridEffectType::WARNINGSPACE)
{

}

CHybrid_WarningSpace::CHybrid_WarningSpace(const CHybrid_WarningSpace& rhs)
	:Super(rhs)
{

}

HRESULT CHybrid_WarningSpace::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybrid_WarningSpace::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybrid_WarningSpace::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CHybrid_WarningSpace::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CHybrid_WarningSpace::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CHybrid_WarningSpace::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CHybrid_WarningSpace::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CHybrid_WarningSpace::Update_CheckState()
{
	_uint iEffectPrefabSize = {0};
	_uint iEffectFinishCount = {0};

	// 숫자 카운팅
	for (auto& Prefab : m_StateModules[m_iCurrentState])
	{
		if (Prefab.first == EHybridModuleType::EFFECT)
		{
			++iEffectPrefabSize;
			if (dynamic_cast<CEffect_WarningCircle*>(Prefab.second))
				iEffectFinishCount += static_cast<CEffect_WarningCircle*>(Prefab.second)->IsEffectFinished();
		}
	}
	// 현재 State 애니메이션 전부 다 끝났으면 다음 STate로
	if (iEffectPrefabSize <= iEffectFinishCount)
		++m_iCurrentState;

	// 만약 이번 State가 END라면.
	if (m_iCurrentState == (_uint)EWarningState::END)
	{
		Set_Dead();
		return;
	}

	// State 변경하기.
	if (m_iPrevState != m_iCurrentState)
	{
		Change_HybridState(m_iCurrentState);
	}
}

void CHybrid_WarningSpace::Change_HybridState(_uint iState)
{
	//	=================	 이전 State에 관한 것		===================

	if (m_iPrevState != (_uint)EWarningState::NONE)
	{
		auto iter = m_StateModules.find(m_iPrevState);
		if (iter != m_StateModules.end())
		{
			for (auto pModule : iter->second)
			{
				if (pModule.first == EHybridModuleType::EFFECT)
				{
					if (dynamic_cast<CEffect_WarningCircle*>(pModule.second))
						static_cast<CEffect_WarningCircle*>(pModule.second)->Disable_VFX();
				}

				//else if (pModule.first == EHybridModuleType::COLLIDER)
				//{

				//}
			}
		}
	}
	
	//	=================	  현재 State에 관한 것		===================
	auto iter = m_StateModules.find(m_iCurrentState);
	if (iter != m_StateModules.end())
	{
		for (auto pModule : iter->second)
		{
			if (pModule.first == EHybridModuleType::EFFECT)
			{
				if (dynamic_cast<CEffect_WarningCircle*>(pModule.second))
					static_cast<CEffect_WarningCircle*>(pModule.second)->Enable_VFX(&m_tSpawnDesc[m_iCurrentState]);
			}

			//else if (pModule.first == EHybridModuleType::COLLIDER)
			//{

			//}
		}
	}

	m_iPrevState = m_iCurrentState;
}

HRESULT CHybrid_WarningSpace::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHybrid_WarningSpace::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	vector<EFFECT_WARNING_DESC>* pDescList = static_cast<vector<EFFECT_WARNING_DESC>*>(pArg);

	if (pDescList == nullptr || pDescList->empty())
		return E_FAIL;

	// vector 전체를 복사 (캐싱)
	m_tSpawnDesc = *pDescList;

	// 초기 상태 설정
	m_iPrevState = ENUM_TO_UINT(EWarningState::NONE);
	m_iCurrentState = ENUM_TO_UINT(EWarningState::WARNING);

	return S_OK;
}

HRESULT CHybrid_WarningSpace::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	// 초기화
	m_iPrevState = m_iCurrentState = ENUM_TO_UINT(EWarningState::NONE);
	m_tSpawnDesc = {};

	return S_OK;
}

CHybrid_WarningSpace* CHybrid_WarningSpace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHybrid_WarningSpace* pInstance = new CHybrid_WarningSpace(pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : Effect");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : Effect");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CHybrid_WarningSpace::Clone(void* pArg)
{
	CHybrid_WarningSpace* pClone = new CHybrid_WarningSpace(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Effect::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CHybrid_WarningSpace::Free()
{
	Super::Free();
}