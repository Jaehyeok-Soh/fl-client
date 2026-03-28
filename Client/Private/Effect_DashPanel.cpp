#include "pch.h"
#include "Effect_DashPanel.h"
#include "EffectObject.h"
#include "GameInstance.h"

CEffect_DashPanel::CEffect_DashPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{

}

CEffect_DashPanel::CEffect_DashPanel(const CEffect_DashPanel& rhs)
	:Super(rhs)
{

}

HRESULT CEffect_DashPanel::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_DashPanel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_DashPanel::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEffect_DashPanel::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CEffect_DashPanel::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEffect_DashPanel::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CEffect_DashPanel::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CEffect_DashPanel::Render()
{
	Super::Render();

	return S_OK;
}

void CEffect_DashPanel::Spawn_PositionCalculate(void* pArg)
{
	EFFECT_SPAWN_DESC* base = static_cast<EFFECT_SPAWN_DESC*>(pArg);
	EFFECT_LINE_DESC* pDesc = dynamic_cast<EFFECT_LINE_DESC*>(base);

	if (!pDesc)
		return;

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			effectObject->Spawn_FromPool(pArg);
	}
}

void CEffect_DashPanel::DashFinished()
{
	// 대쉬가 끝났으면 자식 Loop를 꺼줌으로써 자동으로 풀 회수가 되게 한다.

	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
		{
			auto EO = static_cast<CEffectObject*>(effectObject);
			EO->LoopState_Change(DTO::E_LoopState::LOOP_END);
		}
	}
}

HRESULT CEffect_DashPanel::Spawn_FromPool(void* pArg)
{
	if (nullptr == pArg) return E_FAIL;
	m_bIsEffectFinish = false;

	Spawn_PositionCalculate(pArg);

	return S_OK;
}

HRESULT CEffect_DashPanel::Enable_VFX(void* pArg)
{
	m_bIsEffectFinish = false;

	EFFECT_SPAWN_DESC* base = static_cast<EFFECT_SPAWN_DESC*>(pArg);
	EFFECT_LINE_DESC* pDesc = dynamic_cast<EFFECT_LINE_DESC*>(base);

	if (!pDesc)
		return E_FAIL;

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			static_cast<CEffectObject*>(effectObject)->Enable_VFX(pArg);
	}

	return S_OK;
}

HRESULT CEffect_DashPanel::Disable_VFX()
{
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
		{
			auto EO = static_cast<CEffectObject*>(effectObject);
			EO->Disable_VFX();
		}
	}

	return S_OK;
}

CEffect_DashPanel* CEffect_DashPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffect_DashPanel* pInstance = new CEffect_DashPanel(pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : CEffect_DashPanel");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : CEffect_DashPanel");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEffect_DashPanel::Clone(void* pArg)
{
	CEffect_DashPanel* pClone = new CEffect_DashPanel(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CEffect_DashPanel::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CEffect_DashPanel::Free()
{
	Super::Free();
}