#include "pch.h"
#include "Navigation.h"
#include "StatComponent.h"
#include "MainPlayer.h"
#include "Model.h"
#include "Weapon.h"
#include "ColliderPart.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "CameraMan_Targeter.h"
#include "Collider.h"
#include "Bone.h"
#include "Body.h"
#include "PlayerActionState.h"
#include "GameInstance.h"
#pragma region States
#include "State_RunStart.h"
#include "State_Run.h"
#include "State_RunEnd.h"
#include "State_Idle.h"
#pragma endregion
#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Super(rhs)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_BaseStates()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_Actor_Object(this);
    if (CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>())
        if (FAILED(pPlayerState->Awake(iCurrentLevelID)))
            return E_FAIL;

    return S_OK;
}

void CPlayer::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(const _float fTimeDelta)
{
    if (CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>())
    {
        pPlayerState->Update(fTimeDelta);
    }

    Super::Update(fTimeDelta);
}

void CPlayer::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CPlayer::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CPlayer::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer::Get_AnimationIndex(const wstring& wstrName)
{
    if (CBody* pBody = Get_Part<CBody>(Part::BODY))
    {
        if (CModel* pModel = pBody->Get_Component<CModel>())
        {
            return pModel->Get_AnimationIndex(wstrName);
        }
        return -1;
    }
    return -1;
}

HRESULT CPlayer::Ready_BaseStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    // Idle
    {
        CState_Idle::STATE_DESC desc = {};
        desc.iAnimIndex = Get_AnimationIndex(L"Animation_TestPlayer_Idle");
        desc.bBlend = true;
        desc.bLoop = true;
        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::IDLE), CState_Idle::Create(pActionState, &desc))))
            return E_FAIL;
    }
    // RunStart
    {
        CState_RunStart::STATE_DESC desc = {};
        desc.iAnimIndex = Get_AnimationIndex(L"Animation_TestPlayer_Run_Loop");
        desc.bBlend = true;
        desc.bLoop = true;
        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNSTART), CState_RunStart::Create(pActionState, &desc))))
            return E_FAIL;
    }
    // Run
    {
        CState_Run::STATE_DESC desc = {};
        desc.iAnimIndex = Get_AnimationIndex(L"Animation_TestPlayer_Run_Loop");
        desc.bBlend = true;
        desc.bLoop = true;
        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUN), CState_Run::Create(pActionState, &desc))))
            return E_FAIL;
    }
    // RunEnd
    {
        CState_RunEnd::STATE_DESC desc = {};
        desc.iAnimIndex = Get_AnimationIndex(L"Animation_TestPlayer_Run_Loop");
        desc.bBlend = true;
        desc.bLoop = true;
        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNEND), CState_RunEnd::Create(pActionState, &desc))))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects(PLAYER_DESC* pDesc)
{
    {
        CBody::BODY_DESC bodyDesc = {};
        bodyDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        bodyDesc.iLevelIndex = pDesc->iLevelIndex;
        bodyDesc.wstrModelPrototypeName = pDesc->wstrBodyModelTag;
        if (FAILED(Add_Part(Part::BODY, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Body", &bodyDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{
    {
        CPlayerActionState::ACTIONSTATE_DESC desc = {};
        desc.iStateCount = ENUM_TO_UINT(State::END);
        desc.pOwnerModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
        if (FAILED(Add_Component<CPlayerActionState>(0, L"Prototype_Component_ActionState_Player", &desc)))
            return E_FAIL;
    }

    if (pDesc->wstrNavigationPrototypeTag.empty() == false)
    {
        CNavigation::NAVIGATION_DESC desc = {};
        desc.iCurrentIndex = pDesc->iNavigationCellIndex;
        desc.vPosition = pDesc->vSpawnPosition;
        if (FAILED(Add_Component<CNavigation>(0 /* static */, pDesc->wstrNavigationPrototypeTag, &desc)))
            return E_FAIL;
    }

    return S_OK;
}

void CPlayer::Free()
{
    Super::Free();
}
