#include "Engine_pch.h"
#include "CameraMan.h"
#include "CinematicCameraSequence.h"
#include "GameInstance.h"

CCameraMan::CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType)
    : Super(pDevice, pDeviceContext)
    , m_eType(eType)
    , m_pCinematicSquence{nullptr}
    , m_iCurFrameIndex{0}
    , m_fDeltaTime{0.f}
{
}

CCameraMan::CCameraMan(const CCameraMan& rhs)
    : Super(rhs)
    , m_eType(rhs.m_eType)
    , m_pCinematicSquence{rhs.m_pCinematicSquence }
    , m_iCurFrameIndex{rhs.m_iCurFrameIndex }
    , m_fDeltaTime{rhs.m_fDeltaTime}
{
}

HRESULT CCameraMan::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    if (FAILED(Add_Component<CCamera>(0 , L"Prototype_Component_Camera", pDesc->pCamera_Desc)))
        return E_FAIL;

    if (FAILED(Add_Component<CCameraController>(0, L"Prototype_Component_CameraController", nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CCameraMan::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CCameraMan::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);




    Cinematic(fTimeDelta);
}

void CCameraMan::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    Get_Component<CCameraController>()->Update_Controller(fTimeDelta);
}

void CCameraMan::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    CAMERA_POSE basePose = Capture_BasePose_FromTransform();
    CAMERA_POSE finalPose = basePose;
    Get_Component<CCameraController>()->Build_FinalPose(basePose, finalPose);
    Apply_FinalPose_ToCamera(finalPose);
}

inline void CCameraMan::Change_Actor(CGameObject* pGo)
{
    if (nullptr == pGo)
        return;

    Safe_Release(m_pActor);
    Safe_AddRef(pGo);
    m_pActor = pGo;
}


void CCameraMan::Cinematic(const _float fTimeDelta)
{
    /* 시네마틱 State가 아니라면 움직임 제한 */
    if (!m_pCinematicSquence || m_isCinematicEvent == false) return;

    /* 처음 프레임은 카메라 위치값 */
    /* 마지막 프레임은 의미가 없다 */


    return;
}

void CCameraMan::Cinematic(CinematicCameraSequence* pCameraCinematicSequence)
{
    if (pCameraCinematicSequence == nullptr) return;
    
    /* Test */
    m_pCinematicSquence = pCameraCinematicSequence;
    m_isCinematicEvent = true;

}

void CCameraMan::Play_CameraShake(const CAMERA_SHAKE_DESC& desc)
{
    Get_Component<CCameraController>()->Play_Shake(desc);
}

void CCameraMan::Play_CameraFOV(const CAMERA_FOV_DESC& desc)
{
    Get_Component<CCameraController>()->Play_FOV(desc);
}

void CCameraMan::Play_CameraPositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc)
{
    Get_Component<CCameraController>()->Play_PositionOffset(desc);
}

void CCameraMan::Play_CameraRotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc)
{
    Get_Component<CCameraController>()->Play_RotationOffset(desc);
}

CAMERA_POSE CCameraMan::Capture_BasePose_FromTransform()
{
    CAMERA_POSE tPose{};

    CTransform* pTrasnform  = Get_Component<CTransform>();
    CCamera* pCameraComp    = Get_Component<CCamera>();
    tPose.vPos              = pTrasnform->Get_Info(TRANSFORM_INFO_STATE::POS);
    tPose.vRight            = pTrasnform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    tPose.vUp               = pTrasnform->Get_Info(TRANSFORM_INFO_STATE::UP);
    tPose.vLook             = pTrasnform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    tPose.fFovRad           = pCameraComp->Get_BaseFov();

    tPose.vRight.Normalize();
    tPose.vUp.Normalize();
    tPose.vLook.Normalize();
    return tPose;
}

void CCameraMan::Apply_FinalPose_ToCamera(const CAMERA_POSE& finalPose)
{
    Get_Component<CCamera>()->Update_View(finalPose);
}

void CCameraMan::Free()
{
    Safe_Release(m_pActor);
    Super::Free();
}
