#include "pch.h"
#include "WeaponPickUp.h"
#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "GameInstance.h"

CWeaponPickUp::CWeaponPickUp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CInteractiveObject(pDevice, pDeviceContext)
	, m_isTutorialEvent{false}
	, m_eWeaponType{CWeaponPickUp::EWeaponType::END}
	, m_tShaderRGBColorDesc{}
{
}

CWeaponPickUp::CWeaponPickUp(const CWeaponPickUp& rhs)
	: CInteractiveObject(rhs)
	, m_isTutorialEvent{rhs.m_isTutorialEvent }
	, m_eWeaponType{rhs.m_eWeaponType}
	, m_tShaderRGBColorDesc{rhs.m_tShaderRGBColorDesc }
{
}


HRESULT CWeaponPickUp::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}


HRESULT CWeaponPickUp::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CWeaponPickUp::WEAPONPICKUP_DESC* pDesc{ static_cast<CWeaponPickUp::WEAPONPICKUP_DESC*>(pArg) };

	m_eWeaponType		= CWeaponPickUp::WeaponType_ToEnum(pDesc->strWeaponType);
	m_isTutorialEvent	= pDesc->isTutorialEvent;


	if (FAILED(Ready_RGBColorDesc()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeaponPickUp::Ready_RGBColorDesc()
{
	switch (m_eWeaponType)
	{
	case Client::CWeaponPickUp::EWeaponType::One_Handed_Sword:
	{
		m_tShaderRGBColorDesc.vColorR = Vec4(0.119538f, 0.119538f, 0.119538f, 1.f);
		m_tShaderRGBColorDesc.vColorG = Vec4(1.f, 0.751839f, 0.182292f, 1.f);
		m_tShaderRGBColorDesc.vColorB = Vec4(0.458824f, 0.435294f, 0.45098f, 1.f);
	}
		break;
	case Client::CWeaponPickUp::EWeaponType::Dual_Daggers:
	{
		m_tShaderRGBColorDesc.vColorR = Vec4(0.309524f, 0.309524f, 0.309524f, 1.f);
		m_tShaderRGBColorDesc.vColorG = Vec4(0.10119f, 0.10119f, 0.10119f, 1.f);
		m_tShaderRGBColorDesc.vColorB = Vec4(0.125f, 0.055804f, 0.055804f, 1.f);
	}
		break;
	case Client::CWeaponPickUp::EWeaponType::Machine_gun:
	{
		m_tShaderRGBColorDesc.vColorR = Vec4(0.947917f, 0.947917f, 0.947917f, 1.f);
		m_tShaderRGBColorDesc.vColorG = Vec4(0.364583f, 0.355613f, 0.351292f, 1.f);
		m_tShaderRGBColorDesc.vColorB = Vec4(0.03954f, 0.035601f, 0.03434f, 1.f);
	}
		break;
	default:	return E_FAIL;
	}
	return S_OK;
}

HRESULT CWeaponPickUp::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CWeaponPickUp::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CWeaponPickUp::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CWeaponPickUp::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CWeaponPickUp::Ready_Before_Render(const _float fTimeDelta)
{
	/* TutorialWeapon는 특별하게 해준다 */
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::COMPUTELIGHT_BLEND, this);
}

HRESULT CWeaponPickUp::Render()
{
	CModel*		pModel  = Get_Component<CModel>();		if(!pModel)		return E_FAIL;
	CShader*	pShader = Get_Component<CShader>();		if(!pShader)	return E_FAIL;

	pShader->Bind_TransformData(Get_Component<CTransform>()->Get_WorldMatrix());
	_uint iMeshCount = pModel->Get_MeshCount();

	/* 칼 색깔 칠하기 */
	pShader->Bind_RGBColorData(m_tShaderRGBColorDesc);

	pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::RGBMapping));

	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_MaterialInstance(pShader, i);
		pModel->Bind_Material(pShader,i);
		pShader->Apply();
		pModel->Render(i);
	}


	return S_OK;
}

CWeaponPickUp* CWeaponPickUp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWeaponPickUp* pWeaponPickUp = new CWeaponPickUp(pDevice, pDeviceContext);

	if (FAILED(pWeaponPickUp->Initialize_Prototype()))
	{
		Safe_Release(pWeaponPickUp);
		MSG_BOX("TutorialWeapon is failed to Create");
		return nullptr;
	}
	return pWeaponPickUp;
}

CGameObject* CWeaponPickUp::Clone(void* pArg)
{
	CWeaponPickUp* pTutorialWeapon = new CWeaponPickUp(*this);

	if (FAILED(pTutorialWeapon->Initialize(pArg)))
	{
		Safe_Release(pTutorialWeapon);
		MSG_BOX(" TutorialWeapon is failed to Clone");
		return nullptr;
	}

	return pTutorialWeapon;
}


void CWeaponPickUp::Free()
{
	Super::Free();

}


#pragma region Interact Interface 관련

void CWeaponPickUp::Interact()
{
	Super::Interact();

}
#pragma endregion

#pragma region Quest Interface 관련

void CWeaponPickUp::QuestEnter()
{
	Super::QuestEnter();
}

void CWeaponPickUp::QuestExit()
{
	Super::QuestExit();
}

#pragma endregion


CWeaponPickUp::EWeaponType CWeaponPickUp::WeaponType_ToEnum(const string& strType)
{
	if (strType == "One_Handed_Sword")	return Client::CWeaponPickUp::EWeaponType::One_Handed_Sword;
	if (strType == "Dual_Daggers")		return Client::CWeaponPickUp::EWeaponType::Dual_Daggers;
	if (strType == "Machine_gun")		return Client::CWeaponPickUp::EWeaponType::Machine_gun;

	return Client::CWeaponPickUp::EWeaponType::END;
}

string CWeaponPickUp::WeaponType_ToString(CWeaponPickUp::EWeaponType eType)
{
	switch (eType)
	{
	case Client::CWeaponPickUp::EWeaponType::One_Handed_Sword:	return "One_Handed_Sword";
	case Client::CWeaponPickUp::EWeaponType::Dual_Daggers:		return "Dual_Daggers";
	case Client::CWeaponPickUp::EWeaponType::Machine_gun:		return "Machine_gun";
	default:													break;
	}


	return "Unknonw";
}