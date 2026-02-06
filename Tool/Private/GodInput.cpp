#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"
#include "ImGui_ToolManager.h"
#include "GodInput.h"

CGodInput::CGodInput()
    : Super()
{
}

HRESULT CGodInput::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGodInput::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGodInput::Update(const _float fTimeDelta)
{
    if (KEY_BUTTON_UP(DIK_F5))
        m_isGodStop = !m_isGodStop;

    if (m_isGodStop)
        return;
   

    if (KEY_BUTTON_HOLD(DIK_W))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_Straight(fTimeDelta);
    }
    else if (KEY_BUTTON_HOLD(DIK_S))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_BackWard(fTimeDelta);
    }

    if (KEY_BUTTON_HOLD(DIK_D))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_Right(fTimeDelta);
    }
    else if (KEY_BUTTON_HOLD(DIK_A))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_Left(fTimeDelta);
    }

    if (KEY_BUTTON_HOLD(DIK_LCONTROL))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_Down(Vec3::Up, fTimeDelta);
    }
    else if (KEY_BUTTON_HOLD(DIK_SPACE))
    {
        Get_Owner()->Get_Component<CTransform>()->Go_Up(Vec3::Up, fTimeDelta);
    }

    if (MOUSE_RBUTTON_HOLD)
    {
        if (CImGui_ToolManager::GetInstance()->IsOutofViewport() == false)
        {
            _long iMouseMove = { 0 };
            if (iMouseMove = GET_MOUSE_MOVE_X)
            {
                Get_Owner()->Get_Component<CTransform>()->Turn(Vec3::Up, fTimeDelta * iMouseMove * 0.3f);
            }
            iMouseMove = { 0 };
            if (iMouseMove = GET_MOUSE_MOVE_Y)
            {
                Get_Owner()->Get_Component<CTransform>()->Turn(Get_Owner()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::RIGHT), fTimeDelta * iMouseMove * 0.3f);
            }
        }
    }
}

CGodInput* CGodInput::Create()
{
    CGodInput* pInstance = new CGodInput();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CGodInput::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CGodInput::Clone(void* pArg)
{
    return nullptr;
}

void CGodInput::Free()
{
    Super::Free();
}