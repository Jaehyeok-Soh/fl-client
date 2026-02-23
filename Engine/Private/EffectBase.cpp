#include "Engine_pch.h"
#include "EffectBase.h"
#include "EffectPartBase.h"
#include "GameInstance.h"

CEffectBase::CEffectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{
}

CEffectBase::CEffectBase(const CEffectBase& rhs)
	:Super(rhs)
{
}

HRESULT CEffectBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEffectBase::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CEffectBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEffectBase::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CEffectBase::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CEffectBase::Render()
{
	return S_OK;
}

void CEffectBase::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);
}

void CEffectBase::LoopStateChange(E_LoopState EState)
{
	switch (EState)
	{
	case E_LoopState::LOOP_START:
	{
		for (auto PartObject : m_vecPartObjects)
		{
			static_cast<CEffectPartBase*>(PartObject)->LoopState_Change(CEffectPartBase::E_LoopState::LOOP_START);
		}
	}
	break;
	case E_LoopState::LOOP_END:
	{
		for (auto PartObject : m_vecPartObjects)
		{
			static_cast<CEffectPartBase*>(PartObject)->LoopState_Change(CEffectPartBase::E_LoopState::LOOP_END);
		}
	}
	break;
	}
}

void CEffectBase::Free()
{
	Super::Free();
}
