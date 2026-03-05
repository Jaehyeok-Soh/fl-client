#include "pch.h"
#include "Tool_EffectBase.h"
#include "GameInstance.h"

CTool_EffectBase::CTool_EffectBase(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(eType, pDevice, pDeviceContext)
{
}

CTool_EffectBase::CTool_EffectBase(const CTool_EffectBase& rhs)
	:Super(rhs)
{
}

HRESULT CTool_EffectBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_EffectBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_EffectBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTool_EffectBase::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTool_EffectBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CTool_EffectBase::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTool_EffectBase::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTool_EffectBase::Render()
{
	return S_OK;
}

void CTool_EffectBase::Set_Dead(_bool bStatic)
{
	Super::Set_Dead(bStatic);
}

void CTool_EffectBase::Free()
{
	Super::Free();
}
