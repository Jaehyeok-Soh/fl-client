#include "Character.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	
}

CCharacter::CCharacter(const CCharacter& rhs)
	: Super(rhs)
{
	
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CHARACTER_DESC* pDesc = static_cast<CHARACTER_DESC*>(pArg);


	return S_OK;
}

HRESULT CCharacter::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CCharacter::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CCharacter::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CCharacter::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Free()
{
	Super::Free();
}
