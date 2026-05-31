#include "pch.h"
#include "State_Npctalk.h"

#include "Player.h"
#include "ControlContext.h"
#include "PlayerActionState.h"

#include "GameInstance.h"

CState_Npctalk::CState_Npctalk(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Npctalk")
{
}

HRESULT CState_Npctalk::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Npctalk::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	Bind_Event(iLevelIndex);

	return S_OK;
}

HRESULT CState_Npctalk::Start(void* pArg, _bool bForce)
{
	if (IsRemotePlayer())
	{
		if (FAILED(Super::Start(pArg, bForce)))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_Npctalk::Update(const _float fTimeDelta)
{
	if (IsRemotePlayer())
	{
		Super::Update(fTimeDelta);
		return;
	}

	Super::Update(fTimeDelta);
	

	SetupLook_PointLerp(fTimeDelta, m_vPivot, 5.f);
}

HRESULT CState_Npctalk::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_YLerp(true);

	return S_OK;
}

_bool CState_Npctalk::Can_CheckKey(const _float fTimeDelta)
{
	return false;
}

void CState_Npctalk::Bind_Event(const _uint iCurrentLevelID)
{
	// 대화 끝났을때 -> 상태 전환용 key input 체크
	m_DDialogHandle = m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
		{
			Change_State_byKeyInput();
		});

	// interact 객체의 pivot 받아옴
	m_DInteractObjHandle = m_pGameInstance->Subscribe<INTERACT_DETECT>([this](CGameObject* pObj)
		{
			CTransform* pTransofrm = pObj->Get_Component<CTransform>();
			m_vPivot = pTransofrm->Get_Info(TRANSFORM_INFO_STATE::POS);

			m_vPivot.y = 0.f;
		});
}

void CState_Npctalk::Change_State_byKeyInput()
{
	if (Check_MoveKey(0.f))
		return;

	if (Check_JumpKey(0.f))
		return;

	if (Check_DashKey(0.f))
		return;

	if (Check_CtrlPressKey(0.f))
		return;

	if (Check_CtrlUpKey(0.f))
		return;

	if (Check_MeleeKey(0.f))
		return;

	if (Check_RangeKey(0.f))
		return;

	if (Check_SkillKey(0.f))
		return;

	if (Check_FKey(0.f))
		return;

	Change_PlayerState(STATEKEY::LOOPDONE);
}

CState_Npctalk* CState_Npctalk::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Npctalk* pInstance = new CState_Npctalk(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Npctalk::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Npctalk::Free()
{
	Super::Free();

	m_pGameInstance->Unsubscribe<DIALOGUE_BEGIN>(m_DDialogHandle);
	m_pGameInstance->Unsubscribe<INTERACT_DETECT>(m_DInteractObjHandle);
}
