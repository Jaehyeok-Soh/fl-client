#include "Engine_pch.h"
#include "EffectPartBase.h"
#include "GameInstance.h"

CEffectPartBase::CEffectPartBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{
}

CEffectPartBase::CEffectPartBase(const CEffectPartBase& rhs)
	:Super(rhs)
{
}

HRESULT CEffectPartBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectPartBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectPartBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEffectPartBase::Update_Priority(const _float fDT)
{
	Super::Update_Priority(fDT);
}

void CEffectPartBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEffectPartBase::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CEffectPartBase::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CEffectPartBase::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);
}

void CEffectPartBase::Free()
{
	Super::Free();
}