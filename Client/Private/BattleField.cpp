#include "pch.h"
#include "BattleField.h"

#include "Collider.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


#include "GameInstance.h"

CBattleField::CBattleField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_eFieldType{ CBattleField::Field_Type::END }, m_eFieldState{ CBattleField::Field_State::Idle}
{
}

CBattleField::CBattleField(const CBattleField& rhs)
	: CGameObject(rhs), m_eFieldType{ rhs.m_eFieldType }, m_eFieldState{rhs.m_eFieldState }
{
}

HRESULT CBattleField::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleField::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	CBattleField::BATTLEFIELD_DESC* pDesc = static_cast<CBattleField::BATTLEFIELD_DESC*>(pArg);
	m_eFieldType = pDesc->eFieldType;


	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleField::Ready_Component(BATTLEFIELD_DESC* pDesc)
{
	if (pDesc == nullptr) return E_FAIL;

	const wchar_t*  pwszFindColliderPrototypeTag{L"None"};

	CCollider::COLLIDER_DESC tColliderDesc{};
	CBounding_OBB::BOUNDING_OBB_DESC tBoundingOBB_Desc{};
	CBounding_Sphere::BOUNDING_SPHERE_DESC tBoundingSphere_Desc{};

	/* Box => Obb */
	if (pDesc->eFieldType == CBattleField::Field_Type::Box)
	{
		tBoundingOBB_Desc.vExtents = pDesc->vExtents;
		tColliderDesc.pBoundingDesc = &tBoundingOBB_Desc;
		pwszFindColliderPrototypeTag = g_wszCollider_OBB_Prototype_Tag;
	}
	else if(pDesc->eFieldType == CBattleField::Field_Type::Sphere)
	{
		tBoundingSphere_Desc.fRadius = pDesc->fRadius;
		tColliderDesc.pBoundingDesc = &tBoundingSphere_Desc;
		pwszFindColliderPrototypeTag = g_wszCollider_Sphere_PrototypeTag;
	}

	if (FAILED(Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), pwszFindColliderPrototypeTag, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleField::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CCollider* pCollider = Get_Component<CCollider>();		if (!pCollider) return E_FAIL;
	CTransform* pTs = Get_Component<CTransform>();			if (!pTs)		return E_FAIL;

	pCollider->Update(pTs->Get_WorldMatrix());


	return S_OK;
}

void CBattleField::Update_Priority(const _float fTimeDelta)
{
	if (m_eFieldState != CBattleField::Field_State::Active)
		return;

	Super::Update_Priority(fTimeDelta);
}

void CBattleField::Update(const _float fTimeDelta)
{
	if (m_eFieldState != CBattleField::Field_State::Active)
		return;
	Super::Update(fTimeDelta);


}

void CBattleField::Update_Late(const _float fTimeelta)
{
	if (m_eFieldState != CBattleField::Field_State::Active)
		return;

	Super::Update_Late(fTimeelta);
}

void CBattleField::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);


	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::PRIORITY , this);
}

HRESULT CBattleField::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;


#ifdef _DEBUG

	CCollider* pCollider = Get_Component<CCollider>();
	if (pCollider == nullptr) return E_FAIL;
	pCollider->Render();

#endif // _DEBUG


	return S_OK;
}

CBattleField* CBattleField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattleField* pBattleField = new CBattleField(pDevice, pContext);

	if (FAILED(pBattleField->Initialize_Prototype()))
	{
		Safe_Release(pBattleField);
		MSG_BOX("Battle Field Is Failed To Create");
		return nullptr;
	}

	return pBattleField;
}

CGameObject* CBattleField::Clone(void* pArg)
{
	CBattleField* pBattleField = new CBattleField(*this);

	if (FAILED(pBattleField->Initialize(pArg)))
	{
		Safe_Release(pBattleField);
		MSG_BOX("Battle Field Is Failed To Clone");
		return nullptr;
	}

	return pBattleField;
}

void CBattleField::Free()
{
	Super::Free();
}
