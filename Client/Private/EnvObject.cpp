#include "pch.h"
#include "EnvObject.h"
#include "Effect.h"
#include "GameInstance.h"

CEnvObject::CEnvObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext), m_vecEffect{}
{
}

CEnvObject::CEnvObject(const CEnvObject& rhs)
	: CMapObject(rhs), m_vecEffect{rhs.m_vecEffect}
{
	for (auto& Effect : m_vecEffect)
		Safe_AddRef(Effect);
}

HRESULT CEnvObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEnvObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvObject::Ready_Effect()
{



	return S_OK;
}

HRESULT CEnvObject::Awake(const _uint iCurrentLevelID)
{
	if(FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEnvObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CEnvObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEnvObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);
}

void CEnvObject::Ready_Before_Render(const _float fTimeDelta)
{

	return;
}

HRESULT CEnvObject::Render()
{

	return S_OK;
}



void CEnvObject::Free()
{
	Super::Free();

	/* EnvObject::Free() */
	for (auto& Effect : m_vecEffect)
		Safe_Release(Effect);
}

