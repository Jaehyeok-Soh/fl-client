#include "Client_Defines.h"
#include "ComboState.h"

CComboState::CComboState(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CComboState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	COMBOSTATE_DESC* pDesc = static_cast<COMBOSTATE_DESC*>(pArg);
	m_iRootState = pDesc->iRootState;
	m_iDIK_input = pDesc->iDIK_input;
	m_bLeftMouse = pDesc->bLeftMouse;
	m_fChanceTime_Start = pDesc->fChanceTime_Start;
	m_fChanceTime_End = pDesc->fChanceTime_End;

	return S_OK;
}

HRESULT CComboState::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CComboState::Start(void* pArg, _bool bForce)
{
	if(FAILED(Super::Start(pArg)))
		return E_FAIL;

	m_iDashMask = 0;
	m_iActiveHitWindow = -1;
	m_iActivePart = -1;
	return S_OK;
}

void CComboState::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CComboState::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

_bool CComboState::Is_ChanceTime()
{
	return Is_AnimTrackPositionBetween(m_fChanceTime_Start, m_fChanceTime_End);
}

void CComboState::StartForce_ForAnimation(EDashFlag eFlag, EDir eMove, _float fForceAbs, _float fDragK)
{
	if (Has_Dashed(eFlag))
		return;

	m_iDashMask |= static_cast<_byte>(eFlag);
	switch (eMove)
	{
	case CComboState::EDir::FRONT:
		StartForce_Front_ForAnimation(fForceAbs, fDragK);
		break;
	case CComboState::EDir::BACKWARD:
		StartForce_Backward_ForAnimation(fForceAbs, fDragK);
		break;
	case CComboState::EDir::LEFT:
		StartForce_Left_ForAnimation(fForceAbs, fDragK);
		break;
	case CComboState::EDir::RIGHT:
		StartForce_Right_ForAnimation(fForceAbs, fDragK);
		break;
	default:
		break;
	}
}

void CComboState::Apply_AttackDesc(_int iNewWindow, _int iNewPart, ATTACK_DESC* pDesc)
{
	if (m_iActiveHitWindow == iNewWindow)
		return;

	if (m_iActivePart != -1 && m_iActiveHitWindow != -1)
		Set_AttackCollider(m_iActivePart, false, nullptr);

	if (iNewWindow != -1 && iNewPart != -1)
		Set_AttackCollider(iNewPart, true, pDesc);

	m_iActiveHitWindow = iNewWindow;
	m_iActivePart = iNewPart;
}

void CComboState::Free()
{
	Super::Free();
}
