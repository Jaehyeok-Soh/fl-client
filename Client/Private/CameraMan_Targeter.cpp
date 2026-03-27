#include "pch.h"
#include "CameraMan_Targeter.h"
#include "ContainerObject.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Bone.h"
#include "PhysicsCCT.h"
#include "PhysicsSpringArm.h"

#include "GameInstance.h"

#include "PlayerImguiValues.h"

USING(Client)

CCameraMan_Targeter::CCameraMan_Targeter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext, CameraType::DYNAMIC)
{
}

CCameraMan_Targeter::CCameraMan_Targeter(const CCameraMan_Targeter& rhs)
    : Super(rhs)
    , m_fDistanceSpeed(rhs.m_fDistanceSpeed)
    , m_bChangeFirst(rhs.m_bChangeFirst)
    , m_vTargetPos(rhs.m_vTargetPos)
    , m_fCurLookDistance(rhs.m_fCurLookDistance)
    , m_fCurRightDistance(rhs.m_fCurRightDistance)
{
    m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]    = 0.f;
    m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]       = 0.2f;
    m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]     = 3.3f;

    m_arrGunDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]       = 0.58f;
    m_arrGunDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]          = 0.1f;
    m_arrGunDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]        = 1.f;

    m_arrCurDistances = m_arrNormalDistances;
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

    if (FAILED(Add_Component<CPhysicsSpringArm>(0 /*static*/, L"Prototype_Component_SpringArm", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan_Targeter::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    switch (iCurrentLevelID)
    {
    case ENUM_TO_UINT(ELevelType::KUANGKENG):
    {
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]    = 0.f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]       = 0.5f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]     = 4.0f;
    }
    break;

    case ENUM_TO_UINT(ELevelType::LIANHUO):
    {
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]    = 0.f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]       = 0.3f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]     = 4.7f;
    }
    break;

    default:
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]    = 0.f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]       = 0.2f;
        m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]     = 3.3f;
    }

    Change_CamState(TargeterState::TARGETSYNC);
    //m_fK_SpeedTodist = m_fMaxDistanceDelta / m_fMaxSpeed;

    if (FAILED(Ready_GlobalEvent()))
        return E_FAIL;

    return S_OK;
}

void CCameraMan_Targeter::Update_Priority(const _float fTimeDelta)
{
// debug
    if (KEY_BUTTON_HOLD(DIK_UP))
        m_fCurLookDistance -= fTimeDelta;

    if (KEY_BUTTON_HOLD(DIK_DOWN))
        m_fCurLookDistance += fTimeDelta;

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
    // todo : physic 카메라 충돌시 문제 있을 수 있을수도
    // 우선 플레이어 움직임 처리 이후에 chase를 하도록 하기 위해서 late로 시점을 내림
    Super::Update_Late(fTimeDelta);

    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        break;
    case Client::TargeterState::TARGETSYNC:
        break;
    case Client::TargeterState::GUN:
        //GunCam_Update(fTimeDelta);
        break;
    }
}

void CCameraMan_Targeter::Ready_Before_Render(const _float fTimeDelta)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        break;
    case Client::TargeterState::TARGETSYNC:
        break;
    case Client::TargeterState::GUN:
        GunCam_Update(fTimeDelta);
        break;
    }

    Super::Ready_Before_Render(fTimeDelta);
}

void CCameraMan_Targeter::Initialize_WhenChangeTarget(CGameObject* pTarget)
{
    m_fTargetSpeed = pTarget->Get_Component<CTransform>()->Get_MovePerSec();
}

void CCameraMan_Targeter::Change_CamState(TargeterState eState)
{
    if (m_eCurrentState == eState)
        return;

    m_bChangeFirst = true;

    State_End(m_eCurrentState);
    State_Begin(eState);
    m_eCurrentState = eState;
}

void CCameraMan_Targeter::Change_CamState(_uint iState)
{
    Client::TargeterState eState = static_cast<Client::TargeterState>(iState);
    if (m_eCurrentState == eState)
        return;

    m_bChangeFirst = true;

    State_End(m_eCurrentState);
    State_Begin(eState);
    m_eCurrentState = eState;
}

HRESULT CCameraMan_Targeter::Ready_GlobalEvent()
{
    /* Xibi_Cinematic Event 구독 */
    m_pGameInstance->Subscribe<TUTORIAL_BOSS_CONTATCT>([this]() {
        m_pGameInstance->Play_CameraCinematic(L"Xibi_Cinematic_Cuve");
        Change_CamState(TargeterState::CINEMATIC);
        m_pActor->Set_Active(false);

        return S_OK;
        });

    /* Xibi_Cinematic Event 구독 */
    m_pGameInstance->Subscribe<TUTORIAL_BOSS_CONTATCT_END>([this]() {
        Change_CamState(TargeterState::NORMAL);
        m_pActor->Set_Active(true);
        Vec3 vChangePos = Vec3(339.393f, 270.5f, -323.06f);
        m_pActor->Get_Component<CPhysicsCCT>()->SetFootPosition(vChangePos);

        CGameObject* pBoss = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevelIndex() , g_wszBossLayer , 0 );
        if (pBoss == nullptr) return E_FAIL;
        Vec3 BossPos = pBoss->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
        BossPos.y = 0.f;
        vChangePos.y = 0.f;
        m_pActor->Get_Component<CTransform>()->Look_At_Dir(BossPos - vChangePos);

        static_cast<CPlayer*>(m_pActor)->Change_IdleForce();

        this->Change_CamState(TargeterState::TARGETSYNC);

        return S_OK;
        });


    return S_OK;
}

void CCameraMan_Targeter::Update_Priority_State(const _float fTimeDelta)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        Normal_Update_Priority(fTimeDelta);
        break;

    case Client::TargeterState::TARGETSYNC:
        TargetSync_Update_Priority(fTimeDelta);
        break;

    case Client::TargeterState::GUN:
        GunCam_Update_Priority(fTimeDelta);
        break;

    case Client::TargeterState::SKILL_SEQUENCE:
        Skill_SequeneCam_Update_Priority(fTimeDelta);
        break;

    case Client::TargeterState::TURN:
        TurnCam_Update_Priority(fTimeDelta);
        break;
    }
}

void CCameraMan_Targeter::Update_State(const _float fTimeDelta)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
        Normal_Update(fTimeDelta);
        break;

    case Client::TargeterState::TARGETSYNC:
        TargetSync_Update(fTimeDelta);
        break;

    case Client::TargeterState::GUN:
        //GunCam_Update(fTimeDelta);
        break;

    case Client::TargeterState::SKILL_SEQUENCE:
        Skill_SequeneCam_Update(fTimeDelta);
        break;

    case Client::TargeterState::TURN:
        TurnCam_Update(fTimeDelta);
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
    case Client::TargeterState::GUN:
        GunCam_Begin();
        break;
    case Client::TargeterState::SKILL_SEQUENCE:
        Skill_SequeneCam_Begin();
        break;

    case Client::TargeterState::TURN:
        TurnCam_Begin();
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

    case Client::TargeterState::GUN:
        GunCam_End();
        break;

    case Client::TargeterState::SKILL_SEQUENCE:
        Skill_SequeneCam_End();
        break;

    case Client::TargeterState::TURN:
        TurnCam_End();
        break;
    }
}

void CCameraMan_Targeter::Normal_Begin()
{
    m_fTau_Pos = 0.15f;
}

void CCameraMan_Targeter::Normal_Update_Priority(const _float fTimeDelta)
{
    Change_DistancesAll(fTimeDelta);

    Update_Input(fTimeDelta);
}

void CCameraMan_Targeter::Normal_Update(const _float fTimeDelta)
{
    Chase_Actor(fTimeDelta);
}

void CCameraMan_Targeter::Normal_End()
{
    m_bChaseInit = false;
    m_fStateTime = 0.f;

    m_arrPreDistances = m_arrCurDistances;
}

void CCameraMan_Targeter::TargetSync_Begin()
{
}

void CCameraMan_Targeter::TargetSync_Update_Priority(const _float fTimeDelta)
{
    CGameObject* pActor = Get_Actor();
    if (!pActor)
        return;

    CTransform* pPlayerTransform = pActor->Get_Component<CTransform>();
    CContainerObject* pPlayer = dynamic_cast<CContainerObject*>(pActor);
    if (!pPlayer)
        return;

    Vec3 vChasePositionRaw = Get_CamBoneWorldPos_FromBody(pPlayer->Get_Part<CBody>(0), pPlayerTransform);
    if (!m_bImpactInit)
    {
        m_vChaseFiltered = vChasePositionRaw;
        m_bImpactInit = true;
    }

    _float fT_Chase = 1.f - std::exp(-fTimeDelta / m_fTau_Pos);
    Vec3 vChaseFiltered = Vec3::Lerp(m_vChaseFiltered, vChasePositionRaw, fT_Chase);
    m_vChaseFiltered = vChaseFiltered;

    Vec3 vPlayerLook = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    vPlayerLook.y = 0.0f;

    if (vPlayerLook.LengthSquared() <= g_XMEpsilon.f[0])
        return;

    vPlayerLook.Normalize();

    const _float fYawTarget = std::atan2(vPlayerLook.x, vPlayerLook.z);
    _float fT_Rot = 1.f - std::exp(-fTimeDelta / m_fTau_Rotate);

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

    Vec3 vDesiredPos =       vChasePositionRaw
        + vRight * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]
        - vLook * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]
            + Vec3{ 0.f,1.f,0.f } *m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)];

    CTransform* pCameraTransform = Get_Component<CTransform>();
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vUp);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vDesiredPos);
}

void CCameraMan_Targeter::TargetSync_Update(const _float fTimeDelta)
{
    if (m_fStateTime >= m_fImpactDuration)
    {
        Change_CamState(TargeterState::NORMAL);
    }
}

void CCameraMan_Targeter::TargetSync_End()
{
    m_fYaw_Target       = m_fYaw;
    m_fPitch_Target     = m_fPitch;
    m_bImpactInit       = false;
    m_fStateTime        = 0.f;

    m_arrPreDistances = m_arrCurDistances;
}

void CCameraMan_Targeter::GunCam_Begin()
{

}

void CCameraMan_Targeter::GunCam_Update_Priority(const _float fTimeDelta)
{
    Change_DistancesAll(fTimeDelta);

    Update_Input(fTimeDelta);
}

void CCameraMan_Targeter::GunCam_Update(const _float fTimeDelta)
{
    Chase_Actor(fTimeDelta);
}

void CCameraMan_Targeter::GunCam_End()
{
    m_arrPreDistances = m_arrCurDistances;
}

void CCameraMan_Targeter::Skill_SequeneCam_Begin()
{
}

void CCameraMan_Targeter::Skill_SequeneCam_Update_Priority(const _float fTimeDelta)
{
}

void CCameraMan_Targeter::Skill_SequeneCam_Update(const _float fTimeDelta)
{
    CGameObject* pActor = { nullptr };
    if (!(pActor = Get_Actor()))
        return;

    if (CContainerObject* pObject = dynamic_cast<CContainerObject*>(pActor))
    {
        // 플레이어의 바디를 들고 온다
        CBody* pBodyOfPlayer = nullptr;
        if (!(pBodyOfPlayer = pObject->Get_Part<CBody>(CPlayer::BODY)))
            return;

        // 플레이어의 transform을 들고 온다
        CTransform* pPlayerTransform = nullptr;
        if (!(pPlayerTransform = pObject->Get_Component<CTransform>()))
            return;

        Matrix matFianl;
        // player body의 ""가 있다면 -> bondM * camM
        if (CBone* pCamBone = pBodyOfPlayer->Get_CamBone()) //Get_CamBone ?? Get_CamSocketBone
        {
            matFianl = pCamBone->Get_CombinedTransformMatrix() * pPlayerTransform->Get_WorldMatrix();
        }

        Get_Component<CTransform>()->Set_WorldMatrix(matFianl);
    }
}

void CCameraMan_Targeter::Skill_SequeneCam_End()
{
    //m_fCurDistance = 0.f;
}

void CCameraMan_Targeter::TurnCam_Begin()
{
    CTransform* pTransform = Get_Component<CTransform>();

    Vec3 vLookDir = m_tTurnData.vFirstLookDir;
    vLookDir.y = 0.f;

    // 유효한 방향이 안나오면 내기준 Look
    if (vLookDir.LengthSquared() <= g_XMEpsilon.f[0])
    {
        vLookDir = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
        vLookDir.y = 0.f;
    }
    // 그것도 안되면 걍 world forward
    if (vLookDir.LengthSquared() <= g_XMEpsilon.f[0])
        vLookDir = Vec3::Forward;

    vLookDir.Normalize();

    _float fDistance = m_tTurnData.fDistance;
    if (fDistance <= 0.f)
    {
        Vec3 vCurOffset = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS) - m_tTurnData.vPivot;
        vCurOffset.y = 0.f;
        fDistance = vCurOffset.Length();
        if (fDistance <= g_XMEpsilon.f[0])
            fDistance = 3.f;
    }

    Vec3 vStartPos = m_tTurnData.vPivot - vLookDir * fDistance;
    pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vStartPos);
    pTransform->Look_At_Dir(m_tTurnData.vPivot - vStartPos);
    m_vTurnBaseLookDir = vLookDir;
    m_fTurnBaseDistance = fDistance;
}

void CCameraMan_Targeter::TurnCam_Update_Priority(const _float fTimeDelta)
{

}

void CCameraMan_Targeter::TurnCam_Update(const _float fTimeDelta)
{
    // 기존 적분방식이아닌 Alpha를 이용해서 목표 각도를 바로 계산
    const _float fHalf = (std::max)(0.f, m_tTurnData.fTurnHalfTime);
    const _float fHold = (std::max)(0.f, m_tTurnData.fTurnHoldTime);
    const _float fTotal = fHalf * 2.f + fHold;

    if (m_fStateTime >= fTotal)
    {
        Change_CamState(TargeterState::NORMAL);
        return;
    }

    // Half구간의 목표 회전량은 Speed * TunrHalfTime
    const _float fCurYawDegree = Eval_TurnYawDegree();
    const _float fCurYawRadian = ::XMConvertToRadians(fCurYawDegree);

    // 시작 기준 Offset을 회전
    Vec3 vBaseOffset = -m_vTurnBaseLookDir * m_fTurnBaseDistance;

    Matrix matRot = Matrix::CreateRotationY(fCurYawRadian);
    Vec3 vCurOffset = Vec3::TransformNormal(vBaseOffset, matRot);
    Vec3 vDesiredPosition = m_tTurnData.vPivot + vCurOffset;

    CTransform* pTransform = Get_Component<CTransform>();

    pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vDesiredPosition);

    Vec3 vLook = m_tTurnData.vPivot - vDesiredPosition;
    if (vLook.LengthSquared() > g_XMEpsilon.f[0])
    {
        vLook.Normalize();
        Vec3 vRight = Vec3::Up.Cross(vLook);
        vRight.Normalize();
        Vec3 vUp = vLook.Cross(vRight);
        vUp.Normalize();

        pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
        pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vUp);
        pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook);
    }
}

void CCameraMan_Targeter::TurnCam_End()
{
    CGameObject* pActor = { nullptr };
    if (!(pActor = Get_Actor()))
        return;

    _float fLength = {};

    if (CContainerObject* pObject = dynamic_cast<CContainerObject*>(pActor))
    {
        CBody* pBodyOfPlayer = nullptr;
        if (!(pBodyOfPlayer = pObject->Get_Part<CBody>(CPlayer::BODY)))
            return;

        CTransform* pPlayerTransform = nullptr;
        if (!(pPlayerTransform = pObject->Get_Component<CTransform>()))
            return;

        Vec3 vChasePositionRaw = Get_CamBoneWorldPos_FromBody(pBodyOfPlayer, pPlayerTransform);
        vChasePositionRaw.y = 0.f;

        Vec3 vCurCamPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
        vCurCamPos.y = 0.f;

        fLength = Vec3::Distance(vCurCamPos, vChasePositionRaw);
    }

    m_arrPreDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)] = fLength;
    m_vTurnBaseLookDir = Vec3::Forward;
    m_fTurnBaseDistance = 0.f;
}

void CCameraMan_Targeter::Update_Input(const _float fTimeDelta)
{
    if (!m_pActor)
        return;

    if (m_pGameInstance->ShouldIgnoreMouseDelta())
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
    // 플레이어의 바디를 들고 온다
    CBody* pBodyOfPlayer = nullptr;
    if (!(pBodyOfPlayer = pPlayer->Get_Part<CBody>(CPlayer::BODY)))
        return;

    // 플레이어의 transform을 들고 온다
    CTransform* pPlayerTransform = nullptr;
    if (!(pPlayerTransform = pPlayer->Get_Component<CTransform>()))
        return;

    Vec3 vFinalPosition = Vec3::Zero;
    Vec3 vCurrentPosition = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
    Vec3 vChasePositionRaw = Get_CamBoneWorldPos_FromBody(pBodyOfPlayer, pPlayerTransform);
    if (m_bChaseInit == false)
    {
        m_vChaseFiltered    = vChasePositionRaw;
        m_bChaseInit        = true;
    }

    _float fT_Chase = 1.f - std::exp(-fTimeDelta / m_fTau_Pos);
    Vec3 vChaseFiltered = Vec3::Lerp(m_vChaseFiltered, vChasePositionRaw, fT_Chase);
    m_vChaseFiltered = vChaseFiltered;

    // 보간없이 바로 수행
    m_fPitch = std::clamp(m_fPitch_Target, ::XMConvertToRadians(-85.f), ::XMConvertToRadians(85.f));
    m_fYaw = m_fYaw_Target;

    // z축 회전을 제외하고 rotationM을 만들어 look을 만든다
    Matrix matRotation = Matrix::CreateFromYawPitchRoll(Vec3(m_fPitch, m_fYaw, 0.f));
    Vec3 vLook = Vec3::TransformNormal(Vec3::Backward, matRotation);
    vLook.Normalize();

    /* 내 L 을 이용해 다시 R U을 조립한다*/
    Vec3 vWorldUp = Vec3::Up;
    Vec3 vRight = vWorldUp.Cross(vLook);
    vRight.Normalize();
    Vec3 vUp = vLook.Cross(vRight);
    vUp.Normalize();


    // position : chase의 pos 에서 내 look 방향으로 조금 뒤로 빼
    Vec3 vDesiredPos;

    switch (m_eCurrentState)
    {
    case TargeterState::NORMAL:
        vDesiredPos = vChasePositionRaw // m_vChaseFiltered : 보간으로 쓰고 싶다면
                    + vRight * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]
                    - vLook * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]
                    + Vec3{0.f,1.f,0.f} * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)];
        break;
    case TargeterState::GUN:
        vDesiredPos = vChasePositionRaw
                    + vRight * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]
                    - vLook * m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]
                    + Vec3{ 0.f,1.f,0.f } *m_arrCurDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)];
    break;
    }

    vDesiredPos = CheckCameraCollision(vDesiredPos, vChasePositionRaw); // m_vChaseFiltered : 보간으로 쓰고 싶다면

    // RUL & P 다시 재조립
    CTransform* pCameraTransform = Get_Component<CTransform>();
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::UP,    vUp);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK,  vLook);
    pCameraTransform->Set_Info(TRANSFORM_INFO_STATE::POS,   vDesiredPos);
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
        Change_CamState(TargeterState::NORMAL);
        m_pLockonTarget = nullptr;
    }
}

Vec3 CCameraMan_Targeter::Get_CamBoneWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform)
{
    Matrix matReturn = Matrix::Identity;
    Matrix matWorld = pTrnasform->Get_WorldMatrix(); // player matrix

    CBone* pCamBone = nullptr;

    switch (m_eCurrentState)
    {
    case TargeterState::NORMAL:
    case TargeterState::GUN:
        {
            matReturn = pBody->Get_CamSocketBone()->Get_CombinedTransformMatrix() * matWorld;
            //matReturn = (*(pBody->Get_SocketMatrix(413))) * matWorld;
        }
        break;
    }

    return matReturn.Translation(); // bondM * camM의 Position return
}

_bool CCameraMan_Targeter::Change_Distance(_float fTargetDistance, _float fPreDistance, _float& fCurDistanxe, const _float fTimeDelta)
{
    _float fDistance = fabs(fPreDistance - fTargetDistance);

    // 다른데
    if (fCurDistanxe != fTargetDistance)
    {
        // 현재가 더 작다면
        if (fCurDistanxe < fTargetDistance)
        {
            // 더해줘
            fCurDistanxe += (fDistance / m_fMoveDistanceTime) * fTimeDelta;

            // 근데 커졌다면 조절
            if (fCurDistanxe > fTargetDistance)
            {
                fCurDistanxe = fTargetDistance;
                return true;
            }

        }

        // 현재가 더 크다면
        else
        {
            // 빼줘
            fCurDistanxe -= (fDistance / m_fMoveDistanceTime) * fTimeDelta;

            // 근데 작아졌다면 조절
            if (fCurDistanxe < fTargetDistance)
            {
                fCurDistanxe = fTargetDistance;
                return true;
            }
        }
        return false;
    }

    return true;
}

void CCameraMan_Targeter::Change_DistancesAll(const _float fTimeDelta)
{
    switch (m_eCurrentState)
    {
    case Client::TargeterState::NORMAL:
    {
        for (size_t i = 0; i < ENUM_TO_SZET(DISTANCE_DATA::END); i++)
        {
            Change_Distance(m_arrNormalDistances[i], m_arrPreDistances[i], m_arrCurDistances[i], fTimeDelta);
        }
    }
        break;

    case Client::TargeterState::TARGETSYNC:
    {

    }

        break;

    case Client::TargeterState::GUN:
    {
        for (size_t i = 0; i < ENUM_TO_SZET(DISTANCE_DATA::END); i++)
        {
            Change_Distance(m_arrGunDistances[i], m_arrPreDistances[i], m_arrCurDistances[i], fTimeDelta);
        }
    }
    break;

    case Client::TargeterState::SKILL_SEQUENCE:

        break;
    }
}

void CCameraMan_Targeter::Update_TurnOn(const _float fTimeDelta)
{
    CTransform* pTransform = Get_Component<CTransform>();
    Vec3 vRight = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vRight.Normalize();

    // right로 움직여
    pTransform->Add_Position(vRight * m_tTurnData.fSpeed * fTimeDelta);

    // pivot을 봐
    pTransform->Look_At(m_tTurnData.vPivot);


    //Vec3 vNewPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);

    pTransform->Chase(m_tTurnData.vPivot, m_tTurnData.fDistance, fTimeDelta);

}

void CCameraMan_Targeter::Update_TurnOff(const _float fTimeDelta)
{
    CTransform* pTransform = Get_Component<CTransform>();
    Vec3 vRight = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vRight.Normalize();

    // right로 움직여
    pTransform->Add_Position(vRight * m_tTurnData.fSpeed * fTimeDelta * -1.f);

    // pivot을 봐
    pTransform->Look_At(m_tTurnData.vPivot);

    pTransform->Chase(m_tTurnData.vPivot, m_tTurnData.fDistance, fTimeDelta);
}

_float CCameraMan_Targeter::Eval_TurnYawDegree() const
{
    const _float fHalf = (std::max)(0.f, m_tTurnData.fTurnHalfTime);
    const _float fHold = (std::max)(0.f, m_tTurnData.fTurnHoldTime);

    // Speed 를 Degree / Sec으로 해석
    const _float fTargetYawDegree = m_tTurnData.fSpeed * fHalf;
    if (fHalf <= 0.f)
        return 0.f;

    // 들어갈 때
    if (m_fStateTime < fHalf)
    {
        const _float fT = m_fStateTime / fHalf;
        return fTargetYawDegree * Engine_Utils::EvalEase_EaseOutQuad(fT);
    }
    // 유지
    if (m_fStateTime < fHalf + fHold)
    {
        return fTargetYawDegree;
    }
    // 복귀
    if (m_fStateTime < fHalf * 2.f + fHold)
    {
        const _float fT = (m_fStateTime - fHalf - fHold) / fHalf;
        return fTargetYawDegree * (1.f - Engine_Utils::EvalEase_EaseOutQuad(fT));
    }

    return 0.f;
}

Vec3 CCameraMan_Targeter::CheckCameraCollision(Vec3 vCameraPos, Vec3 vTargetPos)
{
    return Get_Component<CPhysicsSpringArm>()->CheckResolveCollision(vCameraPos, vTargetPos);
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
