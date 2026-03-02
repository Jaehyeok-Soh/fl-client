#include "pch.h"
#include "BossControlContext.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

CBossControlContext::CBossControlContext()
{
}

CBossControlContext::CBossControlContext(const CBossControlContext& rhs)
	: Super(rhs)
	, m_desc(rhs.m_desc)
{
}

HRESULT CBossControlContext::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossControlContext::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
		m_desc = *static_cast<BOSS_CONTROLCONTEXT_DESC*>(pArg);

	return S_OK;
}

HRESULT CBossControlContext::Awake(const _uint iLevelIndex)
{
	if (!(m_pTarget = m_pGameInstance->Get_GameObject_Front(0, L"Player_Layer")))
		return E_FAIL;

	Safe_AddRef(m_pTarget);
	return S_OK;
}

void CBossControlContext::Update(const _float fTimeDelta)
{
    CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();

    m_runtimeDesc.vOwnerPos = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    m_runtimeDesc.vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    m_runtimeDesc.vOwnerRight = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);

    if (m_runtimeDesc.vOwnerLook != Vec3::Zero)
        m_runtimeDesc.vOwnerLook.Normalize();
    if (m_runtimeDesc.vOwnerRight != Vec3::Zero)
        m_runtimeDesc.vOwnerRight.Normalize();

    m_runtimeDesc.bTargetValid = (m_pTarget != nullptr && !m_pTarget->IsDead());

    if (m_runtimeDesc.bTargetValid)
    {
        CTransform* pTargetTr = m_pTarget->Get_Component<CTransform>();
        m_runtimeDesc.vTargetPos = pTargetTr->Get_Info(TRANSFORM_INFO_STATE::POS);
        m_runtimeDesc.vToTarget = m_runtimeDesc.vTargetPos - m_runtimeDesc.vOwnerPos;
        m_runtimeDesc.fDistance = m_runtimeDesc.vToTarget.Length();

        if (m_runtimeDesc.fDistance > 0.0001f)
        {
            m_runtimeDesc.vToTargetDir = m_runtimeDesc.vToTarget / m_runtimeDesc.fDistance;
            m_runtimeDesc.fDotForward = m_runtimeDesc.vOwnerLook.Dot(m_runtimeDesc.vToTargetDir);
        }
        else
        {
            m_runtimeDesc.vToTargetDir = m_runtimeDesc.vOwnerLook;
            m_runtimeDesc.fDotForward = 1.f;
        }
    }
    else
    {
        m_runtimeDesc.vTargetPos = {};
        m_runtimeDesc.vToTarget = {};
        m_runtimeDesc.vToTargetDir = {};
        m_runtimeDesc.fDistance = FLT_MAX;
        m_runtimeDesc.fDotForward = 0.f;
    }
}

void CBossControlContext::UpdateWalk(const _float fTimeDelta)
{
    m_runtimeDesc.vMoveDir = m_runtimeDesc.vOwnerLook;
}

void CBossControlContext::UpdateChase(const _float fTimeDelta)
{
    if (!m_runtimeDesc.bTargetValid)
        return;

    m_runtimeDesc.vMoveDir = m_runtimeDesc.vToTargetDir;
}

void CBossControlContext::Update_8Dir_LocalAxisXZ(const _float fTimeDelta, _float fForward, _float fRight)
{
    Vec3 vDir = m_runtimeDesc.vOwnerLook * fForward + m_runtimeDesc.vOwnerRight * fRight;
    if (vDir != Vec3::Zero)
        vDir.Normalize();

    m_runtimeDesc.vMoveDir = vDir;
}

CBossControlContext* CBossControlContext::Create()
{
	CBossControlContext* pInstance = new CBossControlContext();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CBossControlContext::Create, Failed");
        Safe_Release(pInstance);
    }
	return pInstance;
}

CComponent* CBossControlContext::Clone(void* pArg)
{
    CBossControlContext* pInstance = new CBossControlContext();
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CBossControlContext::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBossControlContext::Free()
{
	Super::Free();
}
