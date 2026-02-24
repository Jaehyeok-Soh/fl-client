#include "pch.h"
#include "UIPrefab.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIPrefab::CUIPrefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIPrefab::CUIPrefab(const CUIPrefab& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIPrefab::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPrefab::Initialize(void* pArg)
{
	PREFAB_MONSTER_NAMEPLATE_DESC* pDesc = static_cast<PREFAB_MONSTER_NAMEPLATE_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPrefab::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIPrefab::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPrefab::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIPrefab::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPrefab::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPrefab::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPrefab::Ready_Components(PREFAB_MONSTER_NAMEPLATE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIPrefab::Bind_ShaderResources()
{
	Super::Bind_ShaderResources();
	return S_OK;
}

void CUIPrefab::Free()
{
	Super::Free();
}
