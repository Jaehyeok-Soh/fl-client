#include "pch.h"
#include "SkillWarningSpace.h"
#include "EffectBase.h"
#include "Effect_WarningCircle.h"
#include "GameInstance.h"

CSkillWarningSpace::CSkillWarningSpace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSkillWarningSpace::CSkillWarningSpace(const CSkillWarningSpace& rhs)
	: Super(rhs)
	, m_tWarnDesc(rhs.m_tWarnDesc)
{
}

HRESULT CSkillWarningSpace::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillWarningSpace::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillWarningSpace::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CSkillWarningSpace::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CSkillWarningSpace::OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	EState eState = static_cast<EState>(m_iCurrentState);
	if (eState != EState::STRIKE)
		return;

	if (pOther == nullptr || pOther->IsDead())
		return;

	if (IsDying() || IsDead())
		return;

	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	desc.tHitInfo = tHitInfo;
	m_pGameInstance->Push_CollidedData(desc);
}

void CSkillWarningSpace::Update_HybridState(const _float fTimeDelta)
{
	if (IsDying() && static_cast<EState>(m_iCurrentState) != EState::DISAPEAR)
	{
		Change_HybridState(ENUM_TO_UINT(EState::DISAPEAR));
		return;
	}

	EState eState = static_cast<EState>(m_iCurrentState);

	switch (eState)
	{
	case EState::WARNING:
		if (Is_StateEffectFinished(ENUM_TO_UINT(EState::WARNING)))
		{
			Change_HybridState(ENUM_TO_UINT(EState::STRIKE));
		} break;
	case EState::STRIKE:
		if (Is_StateEffectFinished(ENUM_TO_UINT(EState::STRIKE)))
		{
			Set_Dying();
			Change_HybridState(ENUM_TO_UINT(EState::DISAPEAR));
		} break;
	case EState::DISAPEAR:
		if (Is_StateEffectFinished(ENUM_TO_UINT(EState::DISAPEAR)))
		{
			Set_Dead();
		}break;
	default:
		break;
	}
}

void CSkillWarningSpace::On_StateEnter(_uint iState)
{
	EState eState = static_cast<EState>(iState);
	switch (eState)
	{
	case EState::WARNING:
		Build_WarningDesc();
		break;
	case EState::STRIKE:
		break;
	case EState::DISAPEAR:
		break;
	default:
		break;
	}
}

void CSkillWarningSpace::On_StateExit(_uint iState)
{
	EState eState = static_cast<EState>(iState);
	switch (eState)
	{
	case EState::WARNING:
		break;
	case EState::STRIKE:
		break;
	case EState::DISAPEAR:
		break;
	default:
		break;
	}
}

void CSkillWarningSpace::On_EffectModuleEnter(CGameObject* pModule)
{
	if (pModule == nullptr)
		return;

	if (static_cast<EState>(m_iCurrentState) == EState::WARNING)
	{
		if (CEffect_WarningCircle* pWarn = dynamic_cast<CEffect_WarningCircle*>(pModule))
		{
			pWarn->Enable_VFX(&m_tWarnDesc);
			return;
		}
	}

	static_cast<Engine::CEffectBase*>(pModule)->Enable_VFX(&m_tDefaultEffectDesc);
}

CSkillWarningSpace* CSkillWarningSpace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillWarningSpace* pInstance = new CSkillWarningSpace(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkillWarningSpace::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Engine::CGameObject* CSkillWarningSpace::Clone(void* pArg)
{
	CSkillWarningSpace* pInstance = new CSkillWarningSpace(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillWarningSpace::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillWarningSpace::Build_WarningDesc()
{
	m_tWarnDesc = {};
	m_tDesc.vSpawnPos.y += 0.01f;
	m_tWarnDesc.VFX_Target_Position = m_tDesc.vSpawnPos;
	m_tWarnDesc.VFX_Scale = Vec3::One;
	m_tWarnDesc.iSimulationType = ENUM_TO_UINT(EFFECT_WARNING_DESC::E_VFX_SIMULTYPE::VFX_WORLD);
}

void CSkillWarningSpace::Free()
{
	Super::Free();
}
