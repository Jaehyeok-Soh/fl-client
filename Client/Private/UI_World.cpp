#include "pch.h"
#include "UI_World.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUI_World::CUI_World(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUI_World::CUI_World(const CUI_World& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUI_World::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_World::Initialize(void* pArg)
{
	WORLD_UI_DESC* pDesc = static_cast<WORLD_UI_DESC*>(pArg);
	pDesc->isWorld = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_World::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	return S_OK;
}

void CUI_World::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUI_World::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUI_World::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUI_World::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUI_World::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Super::Render()))
		return E_FAIL;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_World::Ready_Components(WORLD_UI_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUI_World::Bind_ShaderResources()
{
	return S_OK;
}

void CUI_World::Free()
{
	Super::Free();
}
