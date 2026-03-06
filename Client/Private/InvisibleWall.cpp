#include "pch.h"
#include "InvisibleWall.h"


/* GameInstnace */
#include "GameInstance.h"


CInvisibleWall::CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice, pDeviceContext)
{
}

CInvisibleWall::CInvisibleWall(const CInvisibleWall& rhs)
	: CMapObject(rhs)
{
}


HRESULT CInvisibleWall::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvisibleWall::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	INVISIBLEWALL_DESC*  pDesc= static_cast<INVISIBLEWALL_DESC*>(pArg);

	return S_OK;
}

HRESULT CInvisibleWall::Ready_Component(INVISIBLEWALL_DESC* pDesc)
{
	return S_OK;
}

HRESULT CInvisibleWall::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CInvisibleWall::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CInvisibleWall::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CInvisibleWall::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}

void CInvisibleWall::Ready_Before_Render(const _float fTimeDelta)
{
#ifdef _DEBUG
	if(KEY_BUTTON_HOLD(DIK_9))
		Super::Ready_Before_Render(fTimeDelta);
#endif // _DEBUG
}

HRESULT CInvisibleWall::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(CMapObject::Render_Default(ENUM_TO_UINT(EMapObjectShaderPass::StaticObject))))
		return E_FAIL;

	return S_OK;
}

CInvisibleWall* CInvisibleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CInvisibleWall* pInvisibleWall = new CInvisibleWall(pDevice, pDeviceContext);

	if (FAILED(pInvisibleWall->Initialize_Prototype()))
	{
		Safe_Release(pInvisibleWall);
		MSG_BOX("InvisibleWall is failed to Create");
		return nullptr;
	}
	return pInvisibleWall;
}

CGameObject* CInvisibleWall::Clone(void* pArg)
{
	CInvisibleWall* pInvisibleWall = new CInvisibleWall(*this);

	if (FAILED(pInvisibleWall->Initialize(pArg)))
	{
		Safe_Release(pInvisibleWall);
		MSG_BOX(" InvisibleWall is failed to Clone ");
		return nullptr;
	}

	return pInvisibleWall;
}

void CInvisibleWall::Free()
{
	Super::Free();
}
