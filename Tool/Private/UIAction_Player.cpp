#include "pch.h"
#include "UIAction_Player.h"
#include "Tool_Defines.h"

CUIAction_Player::CUIAction_Player()
{
}

HRESULT CUIAction_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIAction_Player::Initialize(void* pArg)
{
	return S_OK;
}

void CUIAction_Player::Update(const _float fTimeDelta)
{
}

CUIAction_Player* CUIAction_Player::Create()
{

}

CComponent* CUIAction_Player::Clone(void* pArg)
{

}

void CUIAction_Player::Free()
{
	Super::Free();
}



