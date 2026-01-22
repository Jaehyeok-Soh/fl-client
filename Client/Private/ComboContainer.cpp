#include "Client_Defines.h"
#include "GameInstance.h"
#include "ComboState.h"
#include "ComboContainer.h"

CComboContainer::CComboContainer(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
	m_vecComboes.reserve(10);
}

HRESULT CComboContainer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CComboContainer::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	for (auto& pCombo : m_vecComboes)
	{
		if (pCombo)
		{
			if (FAILED(pCombo->Awake(iLevelIndex)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CComboContainer::Start(void* pArg, _bool bForce)
{
	if (m_vecComboes.size() <= 0)
		return E_FAIL;

	m_iCurrentComboIndex = 0;
	m_vecComboes[m_iCurrentComboIndex]->Start(nullptr);
	Setup_Combo(m_vecComboes[m_iCurrentComboIndex]);
	return S_OK;
}

void CComboContainer::Update(const _float fTimeDelta)
{
	if (m_iCurrentComboIndex < 0 || m_iCurrentComboIndex >= m_vecComboes.size())
		return;

	if (!m_vecComboes[m_iCurrentComboIndex])
		return;

	Super::Update(fTimeDelta);
	
	if (m_vecComboes[m_iCurrentComboIndex]->Is_ChanceTime())
	{
		if (Get_Input())
		{
			if (Goto_NextCombo(nullptr))
				return;
		}
	}
	if (m_vecComboes[m_iCurrentComboIndex]->Is_FinishedState())
	{
		Request_Change_State(m_iCurrentRootState);
		return;
	}
	
	m_vecComboes[m_iCurrentComboIndex]->Update(fTimeDelta);
}

HRESULT CComboContainer::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_vecComboes[m_iCurrentComboIndex]->End();
	m_eKeyState = ComboKeyState::NONE;
	m_bCurrentLeftMouseInput = false;
	m_iCurrentComboIndex = -1;
	m_iCurrentRootState = -1;
	m_iCurrentDIKInput = -1;
	//m_fCurrentCollideTime_Start = 0.f;
	//m_fCurrentCollideTime_End = 0.f;
	return S_OK;
}

HRESULT CComboContainer::Add_Comobo(CComboState* pComobo)
{
	if (!pComobo)
		return E_FAIL;

	pComobo->Set_ComboIndex((_uint)m_vecComboes.size());
	m_vecComboes.emplace_back(pComobo);
	return S_OK;
}

void CComboContainer::Setup_Combo(CComboState* pCombo)
{
	if (pCombo->Get_DIK() != -1)
	{
		m_iCurrentDIKInput = pCombo->Get_DIK();
		m_bCurrentLeftMouseInput = false;
		m_eKeyState = ComboKeyState::KEY;
	}
	else
	{
		m_iCurrentDIKInput = -1;
		m_bCurrentLeftMouseInput = pCombo->Is_LeftMouse();
		m_eKeyState = ComboKeyState::MOUSE;
	}
	m_iCurrentRootState = pCombo->Get_RootState();
	//m_fCurrentCollideTime_Start = pCombo->Get_CollideTimeStart();
	//m_fCurrentCollideTime_End = pCombo->Get_CollideTimeEnd();
}

_bool CComboContainer::Goto_NextCombo(void *pArg)
{
	// ÄÞº¸ ¸¶Áö¸·ÀÌ¸é ±×³É Èê·¯°¡°Ô µÒ
	if (m_iCurrentComboIndex + 1 >= m_vecComboes.size())
		return false;

	m_vecComboes[m_iCurrentComboIndex]->End();
	++m_iCurrentComboIndex;
	if (FAILED(m_vecComboes[m_iCurrentComboIndex]->Start(pArg)))
		return false;

	Setup_Combo(m_vecComboes[m_iCurrentComboIndex]);
	return true;
}

_bool CComboContainer::Get_Input()
{
	switch (m_eKeyState)
	{
	case ComboKeyState::MOUSE:
		return m_bCurrentLeftMouseInput == true ? Is_AttackPressed() : false;
	case ComboKeyState::KEY:
		return KEY_BUTTON_HOLD(m_iCurrentDIKInput);
	}

	return false;
}

CComboContainer* CComboContainer::Create(CActionState* pOwnerComponent, const string& strName)
{
	CComboContainer* pInstance = new CComboContainer(pOwnerComponent, strName);
	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("CComboContainer::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComboContainer::Free()
{
	for (auto& pCombo : m_vecComboes)
	{
		Safe_Release(pCombo);
	}
	m_vecComboes.clear();
	Super::Free();
}
