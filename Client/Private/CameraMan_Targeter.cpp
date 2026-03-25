#include "pch.h"
#include "CameraMan_Targeter.h"
#include "ContainerObject.h"
#include "Player.h"
#include "Body.h"
#include "Model.h"
#include "Bone.h"
#include "PhysicsCCT.h"
#include "PhysicsSpringArm.h"
#include "CameraAnchorResolver.h"
#include "CameraShotUtils.h"
#include "GameInstance.h"

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
    m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)]       = 0.f;
    m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]     = 3.f;

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

    if(KEY_BUTTON_HOLD(DIK_DOWN))
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

    // CameraMan에서 역할이 최종 카메라 합성 외에 다른 곹옹 작업이 들어간다면 return;을 하면안됨
    if (m_eCurrentState == Client::TargeterState::SCRIPTED_SHOT)
    {
        CAMERA_POSE basePose = Capture_BasePose_FromTransform();
        CAMERA_POSE finalPose = basePose;

        if (CCameraController* pController = Get_Component<CCameraController>())
        {
            if (m_bUseScriptedOverlay)
                pController->Build_FinalPose_WithOverlay(basePose, m_tScriptedOverlayResult, finalPose);
            else
                pController->Build_FinalPose(basePose, finalPose);
        }

        // overlay의 local/world offset 때문에 최종 위치가 더 밀릴 수 있으므로
        // 마지막 collision을 한 번 더
        finalPose.vPos = CheckCameraCollision(finalPose.vPos, m_vLastScriptedPivotWS);

        Apply_FinalPose_ToCamera(finalPose);
        return;
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

HRESULT CCameraMan_Targeter::Request_PlayScriptedShot(const Engine::SCRIPTED_CAMERA_SHOT_DESC& shotDesc, const Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& bindingDesc)
{
    if (shotDesc.Pivot.fDuration <= 0.f)
        return E_FAIL;

    Release_ScriptedShotBindingObjects();

    m_tScriptedShotDesc = shotDesc;
    m_tScriptedShotBinding = bindingDesc;

    Retain_ScriptedShotBindingObjects();

    Change_CamState(TargeterState::SCRIPTED_SHOT);
    return S_OK;
}

HRESULT CCameraMan_Targeter::Request_StopScriptedShot()
{
    if (m_eCurrentState != TargeterState::SCRIPTED_SHOT)
        return S_FALSE;

    Change_CamState(TargeterState::NORMAL);
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

    case Client::TargeterState::SCRIPTED_SHOT:
        ScriptedShot_Update_Priority(fTimeDelta);
        break;

    case Client::TargeterState::SCRIPTED_RECOVER:
        ScriptedShot_Update_Priority(fTimeDelta);
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

    case Client::TargeterState::SCRIPTED_SHOT:
        ScriptedShot_Update(fTimeDelta);
        break;

    case Client::TargeterState::SCRIPTED_RECOVER:
        ScriptedRecover_Update(fTimeDelta);
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
    case Client::TargeterState::SCRIPTED_SHOT:
        ScriptedShot_Begin();
        break;
    case Client::TargeterState::SCRIPTED_RECOVER:
        ScriptedRecover_Begin();
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

    case Client::TargeterState::SCRIPTED_SHOT:
        ScriptedShot_End();
        break;

    case Client::TargeterState::SCRIPTED_RECOVER:
        ScriptedRecover_End();
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
    m_fYaw_Target = m_fYaw;
    m_fPitch_Target = m_fPitch;
    m_bImpactInit = false;
    m_fStateTime = 0.f;

    m_arrPreDistances = m_arrNormalDistances;
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
        vLookDir = Vec3::Backward;

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
    m_vTurnBaseLookDir = Vec3::Backward;
    m_fTurnBaseDistance = 0.f;
}

void CCameraMan_Targeter::ScriptedShot_Begin()
{
    m_tScriptedShotRuntime = {};
    m_tScriptedShotRuntime.bPlaying = true;
    m_tScriptedShotRuntime.bPause = false;
    m_tScriptedShotRuntime.fElapsed = 0.f;

    m_tScriptedOverlayResult = {};
    m_bUseScriptedOverlay = false;
    m_vLastScriptedPivotWS = Vec3::Zero;

    Initialize_ScriptedShotSnapshot();
}

void CCameraMan_Targeter::ScriptedShot_Update_Priority(const _float fTimeDelta)
{
    
}

void CCameraMan_Targeter::ScriptedShot_Update(const _float fTimeDelta)
{
    if (m_tScriptedShotRuntime.bPlaying == false)
        return;

    if (m_tScriptedShotRuntime.bPause == false)
        m_tScriptedShotRuntime.fElapsed += fTimeDelta;

    const _float fDuration = (std::max)(0.01f, m_tScriptedShotDesc.Pivot.fDuration);
    const _float fEvalTime = std::clamp(m_tScriptedShotRuntime.fElapsed, 0.f, fDuration);

    Engine::CAMERA_ANCHOR_RESULT tPivot{};
    Engine::CAMERA_ANCHOR_RESULT tLookAt{};
    Resolve_ScriptedShotAnchors(tPivot, tLookAt);

    CAMERA_POSE tBasePose{};
    Vec3 vPivotWS{};
    Evaluate_ScriptedShotBasePose(fEvalTime, tPivot, tLookAt, tBasePose, vPivotWS);

    // base 위치 기준 collision
    tBasePose.vPos = CheckCameraCollision(tBasePose.vPos, vPivotWS);

    // controller overlay는 캐시에만 저장
    Evaluate_ScriptedControllerResult(fEvalTime, m_tScriptedOverlayResult);
    m_bUseScriptedOverlay = true;
    m_vLastScriptedPivotWS = vPivotWS;

    // Transform만 갱신
    Apply_CameraPose(tBasePose);

    // Normal로 가기전 Recover 후
    if (m_tScriptedShotRuntime.fElapsed >= fDuration)
    {
        Prepare_RecoverFromScript();

        // 스냅이면
        if (m_tScriptedShotDesc.Recover.eMethod == ECameraShotRecoverMethod::Snap)
        {
            Apply_CameraPose(m_tRecoverRuntime.tTargetPose);
            Change_CamState(TargeterState::NORMAL);
        }
        // 블랜딩해서 회복할거면
        else
        {
            Change_CamState(TargeterState::SCRIPTED_RECOVER);
        }
    }
        
}

void CCameraMan_Targeter::ScriptedShot_End()
{
    m_tScriptedShotRuntime.bPlaying = false;
    m_tScriptedShotRuntime.bPause = false;

    m_tScriptedOverlayResult = {};
    m_bUseScriptedOverlay = false;
    m_vLastScriptedPivotWS = Vec3::Zero;

    Sync_NormalStateFromCurrentPose();
    Release_ScriptedShotBindingObjects();
}

void CCameraMan_Targeter::ScriptedRecover_Begin()
{
}

void CCameraMan_Targeter::ScriptedRecover_Update_Priority(const _float fTimeDelta)
{
}

void CCameraMan_Targeter::ScriptedRecover_Update(const _float fTimeDelta)
{
    m_tRecoverRuntime.fElapsed += fTimeDelta;

    const _float fT =
        std::clamp(m_tRecoverRuntime.fElapsed / m_tRecoverRuntime.fDuration, 0.f, 1.f);

    const _float fEaseT = Eval_RecoverEase(fT);

    CAMERA_POSE tPose = Lerp_CameraPose(
        m_tRecoverRuntime.tStartPose,
        m_tRecoverRuntime.tTargetPose,
        fEaseT);

    Apply_CameraPose(tPose);

    if (fT >= 1.f)
        Change_CamState(TargeterState::NORMAL);
}

void CCameraMan_Targeter::ScriptedRecover_End()
{
    m_tRecoverRuntime = {};
    Sync_NormalStateFromCurrentPose();
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
    case TargeterState::SCRIPTED_SHOT:
    case TargeterState::SCRIPTED_RECOVER:
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

void CCameraMan_Targeter::Initialize_ScriptedShotSnapshot()
{
    CTransform* pTransform = Get_Component<CTransform>();
    if (pTransform == nullptr)
        return;

    // 이전 pose 저장
    m_tScriptedShotRuntime.tPreShotPose = Capture_BasePose_FromTransform();

    Engine::CAMERA_ANCHOR_RESULT tPivot{};
    Engine::CAMERA_ANCHOR_RESULT tLookAt{};

    // resolve 실패하면 기본으루
    if (Resolve_ScriptedShotAnchors(tPivot, tLookAt) == false)
    {
        tPivot.vPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
        tPivot.vRight = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
        tPivot.vUp = pTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
        tPivot.vLook = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
        tLookAt = tPivot;
    }

    m_tScriptedShotRuntime.tStartPivotAnchor = tPivot;
    m_tScriptedShotRuntime.tStartLookAtAnchor = tLookAt;

    // 시작 시점 카메라 축 저장
    m_tScriptedShotRuntime.vStartCamPosWS   = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    m_tScriptedShotRuntime.vStartRight      = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    m_tScriptedShotRuntime.vStartUp         = pTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
    m_tScriptedShotRuntime.vStartLook       = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

    m_tScriptedShotRuntime.vStartRight.Normalize();
    m_tScriptedShotRuntime.vStartUp.Normalize();
    m_tScriptedShotRuntime.vStartLook.Normalize();

    // Basis 결정
    Vec3 vBasisRight{}, vBasisUp{}, vBasisLook{};
    Resolve_ShotBasis(tPivot, vBasisRight, vBasisUp, vBasisLook);
    
    // 시작모드에 따라 Base Offset 결정
    switch (m_tScriptedShotDesc.Start.eMode)
    {
    case Engine::ECameraShotStartMode::InheritCurrent:
    {
        Vec3 vOffset = m_tScriptedShotRuntime.vStartCamPosWS - tPivot.vPos;

        m_tScriptedShotRuntime.vBaseOffsetLocal.x = vOffset.Dot(vBasisRight);
        m_tScriptedShotRuntime.vBaseOffsetLocal.y = vOffset.Dot(vBasisUp);
        m_tScriptedShotRuntime.vBaseOffsetLocal.z = vOffset.Dot(-vBasisLook);
    }
    break;

    case Engine::ECameraShotStartMode::FixedFromPivot:
    {
        m_tScriptedShotRuntime.vBaseOffsetLocal = m_tScriptedShotDesc.Start.vLocaloffset;
    }
    break;
    }

    // 시작 포즈 즉시 적용할지에 대한것
    if (m_tScriptedShotDesc.Start.bApplyStartPoseImmediately)
    {
        CAMERA_POSE tStartPose{};
        Vec3 vPivotWS{};
        Evaluate_ScriptedShotBasePose(0.f, tPivot, tLookAt, tStartPose, vPivotWS);
        tStartPose.vPos = CheckCameraCollision(tStartPose.vPos, vPivotWS);
        Apply_CameraPose(tStartPose);
    }
}

_bool CCameraMan_Targeter::Resolve_ScriptedShotAnchors(OUT CAMERA_ANCHOR_RESULT& outPivot, OUT CAMERA_ANCHOR_RESULT& outLookAt)
{
    CGameObject* pDefaultActor = Get_Actor();

    

    if (m_tScriptedShotDesc.Pivot.bFollowLivePivot)
    {
        _bool bPivotOk = Engine::CCameraAnchorResolver::Resolve(
            m_tScriptedShotBinding.pviot,
            pDefaultActor,
            outPivot);

        if (bPivotOk)
            m_tScriptedShotRuntime.tLastPivotAnchor = outPivot;
        else
            outPivot = m_tScriptedShotRuntime.tLastPivotAnchor;
    }
    else
    {
        outPivot = m_tScriptedShotRuntime.tStartPivotAnchor;
    }
    

    if (m_tScriptedShotBinding.bUseSeparateLookAt)
    {
        if (m_tScriptedShotDesc.Pivot.bFollowLiveLookAt)
        {
            _bool bLookOk = Engine::CCameraAnchorResolver::Resolve(
                m_tScriptedShotBinding.LookAt,
                pDefaultActor,
                outLookAt);

            if (bLookOk)
                m_tScriptedShotRuntime.tLastLookAtAnchor = outLookAt;
            else
                outLookAt = m_tScriptedShotRuntime.tLastLookAtAnchor;
        }
        else
        {
            outLookAt = m_tScriptedShotRuntime.tStartLookAtAnchor;
        }
    }
    else
    {
        outLookAt = outPivot;
        m_tScriptedShotRuntime.tLastLookAtAnchor = outLookAt;
    }

    return true;
}

void CCameraMan_Targeter::Resolve_ShotBasis(const Engine::CAMERA_ANCHOR_RESULT& pivotAnchor, OUT Vec3& outRight, OUT Vec3& outUp, OUT Vec3& outLook)
{
    switch (m_tScriptedShotDesc.Pivot.eBasisMode)
    {
    case Engine::ECameraBasisMode::START_CAMERA:
        outRight = m_tScriptedShotRuntime.vStartRight;
        outUp = m_tScriptedShotRuntime.vStartUp;
        outLook = m_tScriptedShotRuntime.vStartLook;
        break;

    case Engine::ECameraBasisMode::ANCHOR_OWNER:
        outRight = pivotAnchor.vRight;
        outUp = pivotAnchor.vUp;
        outLook = pivotAnchor.vLook;
        break;

    case Engine::ECameraBasisMode::WORLD:
    default:
        outRight = Vec3::Right;
        outUp = Vec3::Up;
        outLook = Vec3::Backward;
        break;
    }

    outRight.Normalize();
    outUp.Normalize();
    outLook.Normalize();
}

Vec3 CCameraMan_Targeter::CheckCameraCollision(Vec3 vCameraPos, Vec3 vTargetPos)
{
    return Get_Component<CPhysicsSpringArm>()->CheckResolveCollision(vCameraPos, vTargetPos);
}

void CCameraMan_Targeter::Evaluate_ScriptedShotBasePose(_float fTime, const CAMERA_ANCHOR_RESULT& pivotAnchor, const CAMERA_ANCHOR_RESULT& lookAtAnchor, OUT CAMERA_POSE& outBasePose, OUT Vec3& outPivotWS)
{
    outBasePose = {};

    Vec3 vBasisRight{}, vBasisUp{}, vBasisLook{};
    Resolve_ShotBasis(pivotAnchor, vBasisRight, vBasisUp, vBasisLook);

    //////////////////////////////
    // Pivot / LookAt 자체 이동
    //////////////////////////////
    Vec3 vPivotPos = pivotAnchor.vPos;
    vPivotPos += vBasisRight * Eval_Channel1D(m_tScriptedShotDesc.Pivot.PivotOffsetX, fTime);
    vPivotPos += vBasisUp * Eval_Channel1D(m_tScriptedShotDesc.Pivot.PivotOffsetY, fTime);
    vPivotPos += vBasisLook * Eval_Channel1D(m_tScriptedShotDesc.Pivot.PivotOffsetZ, fTime);

    Vec3 vLookAtPos = lookAtAnchor.vPos;
    vLookAtPos += vBasisRight * Eval_Channel1D(m_tScriptedShotDesc.Pivot.LookAtOffsetX, fTime);
    vLookAtPos += vBasisUp * Eval_Channel1D(m_tScriptedShotDesc.Pivot.LookAtOffsetY, fTime);
    vLookAtPos += vBasisLook * Eval_Channel1D(m_tScriptedShotDesc.Pivot.LookAtOffsetZ, fTime);

    outPivotWS = vPivotPos;

    //////////////////////////////
    // 시작 기준 camera local offset
    //////////////////////////////
    _float fLocalX = m_tScriptedShotRuntime.vBaseOffsetLocal.x + Eval_Channel1D(m_tScriptedShotDesc.Pivot.LocalX, fTime);
    _float fLocalY = m_tScriptedShotRuntime.vBaseOffsetLocal.y + Eval_Channel1D(m_tScriptedShotDesc.Pivot.LocalY, fTime);
    _float fLocalZ = m_tScriptedShotRuntime.vBaseOffsetLocal.z + Eval_Channel1D(m_tScriptedShotDesc.Pivot.LocalZ, fTime);

    Vec3 vOffsetWS =
        vBasisRight * fLocalX +
        vBasisUp * fLocalY +
        (-vBasisLook) * fLocalZ;

    //////////////////////////////
    // Orbit
    //////////////////////////////
    const _float fOrbitYawRad = XMConvertToRadians(
        Eval_Channel1D(m_tScriptedShotDesc.Pivot.OrbitYawDeg, fTime));

    Matrix matOrbit = Matrix::CreateFromAxisAngle(vBasisUp, fOrbitYawRad);
    vOffsetWS = Vec3::TransformNormal(vOffsetWS, matOrbit);

    Vec3 vCamPos = vPivotPos + vOffsetWS;

    ///////////
    // Look
    ///////////
    Vec3 vLook = vBasisLook;

    if (m_tScriptedShotDesc.Pivot.bLookAtTarget)
    {
        vLook = vLookAtPos - vCamPos;
        if (vLook.LengthSquared() <= g_XMEpsilon.f[0])
            vLook = vBasisLook;
    }

    vLook.Normalize();

    Vec3 vRight = Vec3::Up.Cross(vLook);
    if (vRight.LengthSquared() <= g_XMEpsilon.f[0])
        vRight = vBasisRight;
    vRight.Normalize();

    Vec3 vUp = vLook.Cross(vRight);
    vUp.Normalize();

    outBasePose.vPos = vCamPos;
    outBasePose.vRight = vRight;
    outBasePose.vUp = vUp;
    outBasePose.vLook = vLook;

    outBasePose.fFovRad = Get_Component<CCamera>()->Get_BaseFov();
}

void CCameraMan_Targeter::Evaluate_ScriptedControllerResult(_float fTime, CAMERA_MODIFIER_RESULT& outResult) const
{
    outResult = {};

    const auto& tCtrl = m_tScriptedShotDesc.Controller;

    outResult.fFovDeltaRad =
        ::XMConvertToRadians(Eval_Channel1D(tCtrl.FovDeltaDeg, fTime));

    outResult.fYawOffsetRad =
        ::XMConvertToRadians(Eval_Channel1D(tCtrl.RotYawDeg, fTime));

    outResult.fPitchOffsetRad =
        ::XMConvertToRadians(Eval_Channel1D(tCtrl.RotPitchDeg, fTime));

    outResult.fRollOffsetRad =
        ::XMConvertToRadians(Eval_Channel1D(tCtrl.RotRollDeg, fTime));

    outResult.vLocalPosOffset.x = Eval_Channel1D(tCtrl.LocalPosX, fTime);
    outResult.vLocalPosOffset.y = Eval_Channel1D(tCtrl.LocalPosY, fTime);
    outResult.vLocalPosOffset.z = Eval_Channel1D(tCtrl.LocalPosZ, fTime);
}

void CCameraMan_Targeter::Apply_CameraPose(const CAMERA_POSE& tPose)
{
    CTransform* pTransform = Get_Component<CTransform>();
    if (pTransform == nullptr)
        return;

    pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, tPose.vRight);
    pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, tPose.vUp);
    pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, tPose.vLook);
    pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, tPose.vPos);
}

void CCameraMan_Targeter::Sync_NormalStateFromCurrentPose()
{
    CTransform* pTransform = Get_Component<CTransform>();
    if (pTransform == nullptr)
        return;

    Vec3 vLook = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    vLook.Normalize();

    m_fYaw = std::atan2(vLook.x, vLook.z);
    m_fPitch = std::asin(std::clamp(vLook.y, -1.f, 1.f)) * -1.f;

    m_fYaw_Target = m_fYaw;
    m_fPitch_Target = m_fPitch;

    m_bChaseInit = false;
}

void CCameraMan_Targeter::Retain_ScriptedShotBindingObjects()
{
    if (m_tScriptedShotBinding.pviot.eSource == Engine::ECameraAnchorSource::OBJECT &&
        m_tScriptedShotBinding.pviot.pObject)
    {
        Safe_AddRef(m_tScriptedShotBinding.pviot.pObject);
    }

    if (m_tScriptedShotBinding.LookAt.eSource == Engine::ECameraAnchorSource::OBJECT &&
        m_tScriptedShotBinding.LookAt.pObject)
    {
        Safe_AddRef(m_tScriptedShotBinding.LookAt.pObject);
    }
}

void CCameraMan_Targeter::Release_ScriptedShotBindingObjects()
{
    if (m_tScriptedShotBinding.pviot.eSource == Engine::ECameraAnchorSource::OBJECT &&
        m_tScriptedShotBinding.pviot.pObject)
    {
        Safe_Release(m_tScriptedShotBinding.pviot.pObject);
        m_tScriptedShotBinding.pviot.pObject = nullptr;
    }

    if (m_tScriptedShotBinding.LookAt.eSource == Engine::ECameraAnchorSource::OBJECT &&
        m_tScriptedShotBinding.LookAt.pObject)
    {
        Safe_Release(m_tScriptedShotBinding.LookAt.pObject);
        m_tScriptedShotBinding.LookAt.pObject = nullptr;
    }
}

void CCameraMan_Targeter::Prepare_RecoverFromScript()
{
    m_tRecoverRuntime = {};
    m_tRecoverRuntime.bActive = true;
    m_tRecoverRuntime.fElapsed = 0.f;
    m_tRecoverRuntime.fDuration = (std::max)(0.01f, m_tScriptedShotDesc.Recover.fBlendTime);
    m_tRecoverRuntime.eEase = m_tScriptedShotDesc.Recover.eEase;

    // 현재 transform 기준 pose
    m_tRecoverRuntime.tStartPose = Capture_BasePose_FromTransform();

    switch (m_tScriptedShotDesc.Recover.eTarget)
    {
    case Engine::ECameraShotRecoverTarget::PreshotSnap:
        m_tRecoverRuntime.tTargetPose = m_tScriptedShotRuntime.tPreShotPose;
        break;

    case Engine::ECameraShotRecoverTarget::GameplaySolved:
        m_tRecoverRuntime.tTargetPose = Solve_GameplayReturnPose();
        break;

    default:
        m_tRecoverRuntime.tTargetPose = m_tScriptedShotRuntime.tPreShotPose;
        break;
    }
}

CAMERA_POSE CCameraMan_Targeter::Solve_GameplayReturnPose()
{
    CAMERA_POSE tPose = Capture_BasePose_FromTransform();

    CGameObject* pActor = Get_Actor();
    if (pActor == nullptr)
        return tPose;

    CContainerObject* pPlayer = dynamic_cast<CContainerObject*>(pActor);
    if (pPlayer == nullptr)
        return tPose;

    CBody* pBody = pPlayer->Get_Part<CBody>(CPlayer::BODY);
    CTransform* pPlayerTransform = pPlayer->Get_Component<CTransform>();
    if (pBody == nullptr || pPlayerTransform == nullptr)
        return tPose;

    Vec3 vPivot = Get_CamBoneWorldPos_FromBody(pBody, pPlayerTransform);

    Matrix matRotation = Matrix::CreateFromYawPitchRoll(Vec3(m_fPitch_Target, m_fYaw_Target, 0.f));

    Vec3 vLook = Vec3::TransformNormal(Vec3::Backward, matRotation);
    vLook.Normalize();

    Vec3 vRight = Vec3::Up.Cross(vLook);
    vRight.Normalize();

    Vec3 vUp = vLook.Cross(vRight);
    vUp.Normalize();

    Vec3 vPos =
        vPivot
        + vRight * m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::RIGHT)]
        - vLook * m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::LOOK)]
        + Vec3::Up * m_arrNormalDistances[ENUM_TO_SZET(DISTANCE_DATA::UP)];

    vPos = CheckCameraCollision(vPos, vPivot);

    tPose.vPos = vPos;
    tPose.vRight = vRight;
    tPose.vUp = vUp;
    tPose.vLook = vLook;

    if (CCamera* pCamera = Get_Component<CCamera>())
        tPose.fFovRad = pCamera->Get_BaseFov();

    return tPose;
}

_float CCameraMan_Targeter::Eval_RecoverEase(_float fT)
{
    fT = std::clamp(fT, 0.f, 1.f);

    switch (m_tRecoverRuntime.eEase)
    {
    case Engine::ECameraShotEase::Linear:
        return fT;
    case Engine::ECameraShotEase::SmoothStep:
        return Engine_Utils::EvalEase_SmoothStep(fT);
    case Engine::ECameraShotEase::EaseOutQuad:
        return Engine_Utils::EvalEase_EaseOutQuad(fT);
    case Engine::ECameraShotEase::EaseInOutQuad:
        return Engine_Utils::EvalEase_EaseInOutQuad(fT);
    case Engine::ECameraShotEase::EaseOutBack:
        return Engine_Utils::EvalEase_EaseOutBack(fT);
    }

    return fT;
}

CAMERA_POSE CCameraMan_Targeter::Lerp_CameraPose(const CAMERA_POSE& tA, const CAMERA_POSE& tB, _float fT) const
{
    CAMERA_POSE out = {};

    out.vPos = Vec3::Lerp(tA.vPos, tB.vPos, fT);

    Quat qA = Quat::CreateFromRotationMatrix(Matrix(
        tA.vRight.x, tA.vRight.y, tA.vRight.z, 0.f,
        tA.vUp.x, tA.vUp.y, tA.vUp.z, 0.f,
        tA.vLook.x, tA.vLook.y, tA.vLook.z, 0.f,
        0.f, 0.f, 0.f, 1.f));

    Quat qB = Quat::CreateFromRotationMatrix(Matrix(
        tB.vRight.x, tB.vRight.y, tB.vRight.z, 0.f,
        tB.vUp.x, tB.vUp.y, tB.vUp.z, 0.f,
        tB.vLook.x, tB.vLook.y, tB.vLook.z, 0.f,
        0.f, 0.f, 0.f, 1.f));

    if (qA.Dot(qB) < 0.f)
        qB = -qB;

    Quat q = Quat::Slerp(qA, qB, fT);
    q.Normalize();

    Matrix mat = Matrix::CreateFromQuaternion(q);

    out.vRight = mat.Right();
    out.vUp = mat.Up();
    out.vLook = mat.Backward();

    out.vRight.Normalize();
    out.vUp.Normalize();
    out.vLook.Normalize();

    out.fFovRad = std::lerp(tA.fFovRad, tB.fFovRad, fT);
    return out;
}

void CCameraMan_Targeter::Debug_PlayScriptedShot(const SCRIPTED_CAMERA_SHOT_DESC& tDesc, const SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding)
{
    Request_PlayScriptedShot(tDesc, tBinding);
}

void CCameraMan_Targeter::Debug_StopScriptedShot()
{
    Request_StopScriptedShot();
}

void CCameraMan_Targeter::Debug_SetScriptedShotTime(_float fTime)
{
    if (m_eCurrentState != TargeterState::SCRIPTED_SHOT)
        return;

    const _float fDuration = (std::max)(0.01f, m_tScriptedShotDesc.Pivot.fDuration);
    m_tScriptedShotRuntime.fElapsed = std::clamp(fTime, 0.f, fDuration);
    m_tScriptedShotRuntime.bPause = true;
}

void CCameraMan_Targeter::Debug_SetScriptedShotPause(_bool bPause)
{
    if (m_eCurrentState != TargeterState::SCRIPTED_SHOT)
        return;

    m_tScriptedShotRuntime.bPause = bPause;
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
    Release_ScriptedShotBindingObjects();
    Safe_Release(m_pLockonTarget);
    Super::Free();
}