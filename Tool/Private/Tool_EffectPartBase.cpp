#include "pch.h"
#include "Tool_EffectPartBase.h"
#include "GameInstance.h"

CTool_EffectPartBase::CTool_EffectPartBase(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(eType, pDevice, pDeviceContext)
{
}

CTool_EffectPartBase::CTool_EffectPartBase(const CTool_EffectPartBase& rhs)
	:Super(rhs)
{
}

HRESULT CTool_EffectPartBase::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_EffectPartBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_EffectPartBase::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTool_EffectPartBase::Update_Priority(const _float fDT)
{
	Super::Update_Priority(fDT);
}

void CTool_EffectPartBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CTool_EffectPartBase::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTool_EffectPartBase::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CTool_EffectPartBase::Set_Dead(_bool bStatic)
{
	Super::Set_Dead(bStatic);
}

void CTool_EffectPartBase::Free()
{
	Super::Free();
}