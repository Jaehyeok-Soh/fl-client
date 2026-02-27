#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "WorldUI_Component.h"
#include "GameInstance.h"

#define ZREF 10.f
#define MAX_SCALE_OFFSET 1.f
#define MIN_SCALE_OFFSET 0.3f


CWorldUI_Component::CWorldUI_Component()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CWorldUI_Component::CWorldUI_Component(const CWorldUI_Component& rhs)
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWorldUI_Component::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldUI_Component::Awake(_uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldUI_Component::Initialize(void* pArg)
{
	WOLRD_UI_COMP_DESC* pDesc = static_cast<WOLRD_UI_COMP_DESC*>(pArg);
	m_pTargetObject = pDesc->pTargetObject;
	m_fVPWidth		= pDesc->fVPWidth;
	m_fVPHegiht		= pDesc->fVPHegiht;
	m_fVPTopLeftX	= pDesc->fVPTopLeftX;
	m_fVPTopLeftY	= pDesc->fVPTopLeftY;
	m_fInitOffset = pDesc->fInitOffset;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	m_WorldProjMatrix = m_pGameInstance->Get_ProjMatrix();
	return S_OK;
}

void CWorldUI_Component::Update(const _float fTimeDelta)
{
	Proj_World_To_Screen();
}

void CWorldUI_Component::Proj_World_To_Screen()
{
	Vec3 vWorldPos = {};
	if (nullptr != m_pTargetObject)
	{
		vWorldPos = m_pTargetObject->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	}
	else
	{
		vWorldPos = m_vTargetPos;
	}

	Vec4 clip = Vec4(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f);
	clip = Vec4::Transform(clip, m_pGameInstance->Get_ViewMatrix());
	m_fViewZ = clip.z;
	clip = Vec4::Transform(clip, m_pGameInstance->Get_ProjMatrix());
	if (clip.w <= 0.0001f)
		return;
	clip /= clip.w;

	Calc_Perspective();
	m_vScreenPos.x = floor(((clip.x * 0.5f + 0.5f) * m_fVPWidth + m_fVPTopLeftX) + m_fInitOffset.x * m_fScaleOffset);
	m_vScreenPos.y = floor(((1.f - (clip.y * 0.5f + 0.5f)) * m_fVPHegiht + m_fVPTopLeftY) + m_fInitOffset.y * m_fScaleOffset);
}

void CWorldUI_Component::Calc_Perspective()
{
	if (m_isRequestScaleOffset)
	{
		m_fScaleOffset = m_fRequestScaleOffset;
	}
	else
	{
		m_fScaleOffset = ZREF / m_fViewZ;
		m_fScaleOffset = std::clamp(m_fScaleOffset, MIN_SCALE_OFFSET, MAX_SCALE_OFFSET);
	}

	m_isRequestScaleOffset = false;
}

void CWorldUI_Component::Request_ScaleOffset(const _float fScale)
{
	m_isRequestScaleOffset = true;
	m_fRequestScaleOffset = fScale;
}

CWorldUI_Component* CWorldUI_Component::Create()
{
	CWorldUI_Component* pInstance = new CWorldUI_Component();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CWorldUI_Component::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CWorldUI_Component::Clone(void* pArg)
{
	CWorldUI_Component* pInstance = new CWorldUI_Component();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CWorldUI_Component::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWorldUI_Component::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
