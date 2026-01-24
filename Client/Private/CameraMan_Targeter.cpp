#include "pch.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "ContainerObject.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Bone.h"
#include "CameraMan_Targeter.h"

USING(Client)

CCameraMan_Targeter::CCameraMan_Targeter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext, CameraType::DYNAMIC)
{
}

CCameraMan_Targeter::CCameraMan_Targeter(const CCameraMan_Targeter& rhs)
    : Super(rhs)
{
}

HRESULT CCameraMan_Targeter::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan_Targeter::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan_Targeter::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    Change_State(TargeterState::NORMAL);
    //m_fK_SpeedTodist = m_fMaxDistanceDelta / m_fMaxSpeed;
    return S_OK;
}

void CCameraMan_Targeter::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
    Update_Priority_State(fTimeDelta);
}

void CCameraMan_Targeter::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
    m_fStateTime += fTimeDelta;
    Update_State(fTimeDelta);
}

void CCameraMan_Targeter::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CCameraMan_Targeter::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

void CCameraMan_Targeter::Initialize_WhenChangeTarget(CGameObject* pTarget)
{
    m_fTargetSpeed = pTarget->Get_Component<CTransform>()->Get_MovePerSec();
}

void CCameraMan_Targeter::Change_State(TargeterState eState)
{
    if (m_eCurrentState == eState)
        return;

    State_End(m_eCurrentState);
    State_Begin(eState);
    m_eCurrentState = eState;
}

void CCameraMan_Targeter::Update_Priority_State(const _float fDeltaTime)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        Normal_Update_Priority(fDeltaTime);
        break;
    case Client::TargeterState::TARGETSYNC:
        TargetSync_Update_Priority(fDeltaTime);
        break;
    }
}

void CCameraMan_Targeter::Update_State(const _float fDeltaTime)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        Normal_Update(fDeltaTime);
        break;
    case Client::TargeterState::TARGETSYNC:
        TargetSync_Update(fDeltaTime);
        break;
    }
}

void CCameraMan_Targeter::State_Begin(TargeterState eState)
{
    switch (eState)
    {
    case Client::TargeterState::NORMAL:
        Normal_Begin();
        break;
    case Client::TargeterState::TARGETSYNC:
        TargetSync_Begin();
        break;
    }
}

void CCameraMan_Targeter::State_End(TargeterState eState)
{
    switch (eState)
    {
    case Client::TargeterState::NORMAL:
        Normal_End();
        break;
    case Client::TargeterState::TARGETSYNC:
        TargetSync_End();
        break;
    }
}

void CCameraMan_Targeter::Normal_Begin()
{
    m_fTau_Pos = 0.15f;
}

void CCameraMan_Targeter::Normal_Update_Priority(const _float fDeltaTime)
{
    Chase_Actor(fDeltaTime);
}

void CCameraMan_Targeter::Normal_Update(const _float fDeltaTime)
{
    Update_Input(fDeltaTime);
}

void CCameraMan_Targeter::Normal_End()
{
    m_bChaseInit = false;
    m_fStateTime = 0.f;
}

void CCameraMan_Targeter::TargetSync_Begin()
{
}

void CCameraMan_Targeter::TargetSync_Update_Priority(const _float fDeltaTime)
{
    CGameObject* pActor = Get_Actor();
    if (!pActor)
        return;

    CTransform* pPlayerTransform = pActor->Get_Component<CTransform>();
    CContainerObject* pPlayer = dynamic_cast<CContainerObject*>(pActor);
    if (!pPlayer)
        return;

    Vec3 vChasePositionRaw = Get_HeadWorldPos_FromBody(pPlayer->Get_Part<CBody>(0), pPlayerTransform);
    if (!m_bImpactInit)
    {
        m_vChaseFiltered = vChasePositionRaw;
        m_bImpactInit = true;
    }

    _float fT_Chase = 1.f - std::exp(-fDeltaTime / m_fTau_Pos);
    Vec3 vChaseFiltered = Vec3::Lerp(m_vChaseFiltered, vChasePositionRaw, fT_Chase);
    m_vChaseFiltered = vChaseFiltered;

    Vec3 vPlayerLook = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    vPlayerLook.Normalize();
    vPlayerLook.y = 0.0f;

    if (vPlayerLook.LengthSquared() <= g_XMEpsilon.f[0])
        return;

    vPlayerLook.Normalize();

    const _float fYawTarget = std::atan2(vPlayerLook.x, vPlayerLook.z);
    _float fT_Rot = 1.f - std::exp(-fDeltaTime / m_fTau_Rotate);

    Quat qCurrent = Quat::CreateFromYawPitchRoll(Vec3(m_fPitch, m_fYaw, 0.f));
    Quat qTarget = Quat::CreateFromYawPitchRoll(Vec3(m_fPitch, fYawTarget, 0.f));

    if (qCurrent.Dot(qTarget) < 0.f)
        qTarget = -qTarget;

    Quat qNew = Quat::Slerp(qCurrent, qTarget, fT_Rot);
    qNew.Normalize();

    Matrix matRotation = Matrix::CreateFromQuaternion(qNew);

    Vec3 vRight = Vec3::TransformNormal(Vec3::Right, matRotation);
    Vec3 vUp = Vec3::TransformNormal(Vec3::Up, matRotation);
    Vec3 vLook = Vec3::TransformNormal(Vec3::Backward, matRotation);
    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();

    m_fYaw = std::atan2(vLook.x, vLook.z);
    m_fPitch = std::asin(std::clamp(vLook.y, -1.f, 1.f)) * -1.f; 

    Vec3 vDesiredPos = vChaseFiltered - vLook * m_fDistance;

    CTransform* pCameraTransform = Get_Component<CTransform>();
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vUp);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vDesiredPos);
}

void CCameraMan_Targeter::TargetSync_Update(const _float fDeltaTime)
{
    if (m_fStateTime >= m_fImpactDuration)
    {
        Change_State(TargeterState::NORMAL);
    }
}

void CCameraMan_Targeter::TargetSync_End()
{
    m_fYaw_Target = m_fYaw;
    m_fPitch_Target = m_fPitch;
    m_bImpactInit = false;
    m_fStateTime = 0.f;
}

void CCameraMan_Targeter::Update_Input(const _float fTimeDelta)
{
    if (!m_pActor)
        return;

    _long iMouseMove = {};
    if (iMouseMove = GET_MOUSE_MOVE_X)
    {
        m_fYaw_Target += iMouseMove * m_fSens * fTimeDelta;
    }
    if (iMouseMove = GET_MOUSE_MOVE_Y)
    {
        m_fPitch_Target = std::clamp(m_fPitch_Target + iMouseMove * m_fSens * fTimeDelta, ::XMConvertToRadians(- 85.f), ::XMConvertToRadians(85.f));
    }
}

void CCameraMan_Targeter::Chase_Actor(const _float fTimeDelta)
{
    CGameObject* pActor = { nullptr };
    if (!(pActor = Get_Actor()))
        return;

    if (CContainerObject* pObject = dynamic_cast<CContainerObject*>(pActor))
    {
        Chase_Player(pObject, fTimeDelta);
    }
}

void CCameraMan_Targeter::Chase_Player(CContainerObject* pPlayer, const _float fTimeDelta)
 {
    CBody* pBodyOfPlayer = nullptr;
    if (!(pBodyOfPlayer = pPlayer->Get_Part<CBody>(CPlayer::BODY)))
        return;

    CTransform* pPlayerTransform = nullptr;
    if (!(pPlayerTransform = pPlayer->Get_Component<CTransform>()))
        return;

    Vec3 vFinalPosition = Vec3::Zero;
    Vec3 vCurrentPosition = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
    Vec3 vChasePositionRaw = Get_HeadWorldPos_FromBody(pBodyOfPlayer, pPlayerTransform);
    if (m_bChaseInit == false)
    {
        m_vChaseFiltered = vChasePositionRaw;
        m_bChaseInit = true;
    }

    _float fT_Chase = 1.f - std::exp(-fTimeDelta / m_fTau_Pos);
    Vec3 vChaseFiltered = Vec3::Lerp(m_vChaseFiltered, vChasePositionRaw, fT_Chase);
    m_vChaseFiltered = vChaseFiltered;

    // 보간없이 바로 수행
    m_fPitch = std::clamp(m_fPitch_Target, ::XMConvertToRadians(-85.f), ::XMConvertToRadians(85.f));
    m_fYaw = m_fYaw_Target;

    Matrix matRotation = Matrix::CreateFromYawPitchRoll(Vec3(m_fPitch, m_fYaw, 0.f));
    Vec3 vLook = Vec3::TransformNormal(Vec3::Backward, matRotation);
    vLook.Normalize();
    Vec3 vWorldUp = Vec3::Up;
    Vec3 vRight = vWorldUp.Cross(vLook);
    vRight.Normalize();
    Vec3 vUp = vLook.Cross(vRight);
    vUp.Normalize();

    Vec3 vDesiredPos = vChaseFiltered - vLook * m_fDistance;

    CTransform* pCameraTransform = Get_Component<CTransform>();
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vUp);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vDesiredPos);
}

void CCameraMan_Targeter::OnChangeLockonTarget(CGameObject* pGo)
{
    if(pGo == m_pLockonTarget)
        return;

    if (m_pLockonTarget)
        Safe_Release(m_pLockonTarget);

    if (pGo)
    {
        m_pLockonTarget = pGo;
        Safe_AddRef(m_pLockonTarget);
    }
    else
    {
        Change_State(TargeterState::NORMAL);
        m_pLockonTarget = nullptr;
    }
}

Vec3 CCameraMan_Targeter::Get_HeadWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform)
{
    Matrix matReturn = Matrix::Identity;
    Matrix matWorld = pTrnasform->Get_WorldMatrix();

    if (CBone* pHead = pBody->Get_HeadBone())
        matReturn = pHead->Get_BindPoseTransformMatrix() * matWorld;
    else if (CBone* pNeck = pBody->Get_NeckBone())
        matReturn = pNeck->Get_BindPoseTransformMatrix() * matWorld;
    else if (CBone* pSpine = pBody->Get_Spine1Bone())
        matReturn = pSpine->Get_BindPoseTransformMatrix() * matWorld;

    return matReturn.Translation();
}

CCameraMan_Targeter* CCameraMan_Targeter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CCameraMan_Targeter* pInstance = new CCameraMan_Targeter(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CCameraMan_Targeter::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCameraMan_Targeter::Clone(void* pArg)
{
    CCameraMan_Targeter* pInstance = new CCameraMan_Targeter(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CCameraMan_Targeter::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCameraMan_Targeter::Free()
{
    Safe_Release(m_pLockonTarget);
    Super::Free();
}
