#include "Engine_pch.h"
#include "CameraMan.h"
#include "GameData_Struct.h"
#include "GameInstance.h"

CCameraMan::CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType)
    : Super(pDevice, pDeviceContext)
    , m_eType(eType)
    , m_vCamShakingOffsetPosition{Vec3::Zero}
    , m_pCinematicSquence{nullptr}
    , m_iCurFrameIndex{0}
    , m_fDeltaTime{0.f}
{
}

CCameraMan::CCameraMan(const CCameraMan& rhs)
    : Super(rhs)
    , m_eType(rhs.m_eType)
    , m_pCinematicSquence{rhs.m_pCinematicSquence }
    , m_vCamShakingOffsetPosition{ rhs.m_vCamShakingOffsetPosition }
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

    Camera_Shaking(fTimeDelta);
}

void CCameraMan::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    Get_Component<CCamera>()->Update_View();
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

    m_fDeltaTime += fTimeDelta;

    CCamera* pCameraCom = Get_Component<CCamera>(); if (pCameraCom == nullptr) return;
    CTransform* pTs = Get_Component<CTransform>();  if (pTs == nullptr) return;

    /* 현재 키프레임 데이터 */
    _uint iSize = static_cast<_uint>(m_pCinematicSquence->vecCamKeyFrameData.size());


    if (m_iCurFrameIndex + 1 >= iSize)
    {
        /* Level_Type */
        m_isCinematicEvent = false;
        m_fDeltaTime = 0;
        m_iCurFrameIndex = 0;
        m_pCinematicSquence = nullptr;
        return;
    }

    auto& tStartCamKeyFrameData  = m_pCinematicSquence->vecCamKeyFrameData[m_iCurFrameIndex];
    auto& tEndCamKeyFrameData    = m_pCinematicSquence->vecCamKeyFrameData[m_iCurFrameIndex + 1];

    /* World 가져와서 적용 */
    Matrix  CurCamWorldMatrix = pTs->Get_WorldMatrix();
    Vec3    vCurCamScale{}, vCurCamPos{};
    Quat    vCurCamQuat{};
    CurCamWorldMatrix.Decompose(vCurCamScale , vCurCamQuat , vCurCamPos);

    /* Start */
    Matrix  CinematicStartWorldMatrix = tStartCamKeyFrameData.Get_WorldMatrix();
    Vec3    vStartCinematicScale{}, vStartCinematicPos{};  Quat    vStartCinematicQuat{};
    CinematicStartWorldMatrix.Decompose(vStartCinematicScale, vStartCinematicQuat, vStartCinematicPos);

    /* End */
    Matrix  CinematicEndWorldMatrix = tEndCamKeyFrameData.Get_WorldMatrix();
    Vec3    vEndCinematicScale{}, vEndCinematicPos{};  Quat    vEndCinematicQuat{};
    CinematicEndWorldMatrix.Decompose(vEndCinematicScale, vEndCinematicQuat, vEndCinematicPos);

    /* Start Index Frame Key에서  시간 비율 다음 프레임의 Duration 값이랑 비교가 된다 */
    _float fTimeRatio = m_fDeltaTime / tEndCamKeyFrameData.fDuration;
    if (fTimeRatio > 1.0f) fTimeRatio = 1.0f;

    _float fReusltRatio{ 0.f };
    Vec3   vResultPos{};
    Quat   vResultQuat{};

    /* Mode Ratio */
    switch (tEndCamKeyFrameData.eMoveLerpType)
    {
    case ELerpType::NONE:
        
        vResultPos = vEndCinematicPos;   /* None 일때는 바로이동 */
        break;
    case ELerpType::Linear:             /* 단순 이동 */
        fReusltRatio = fTimeRatio;
        vResultPos = Vec3::Lerp(vStartCinematicPos, vEndCinematicPos, fReusltRatio);
        break;
    case ELerpType::SmoothStep:
        fReusltRatio = fTimeRatio * fTimeRatio * (3.0f - 2.0f * fTimeRatio);          /* 스무스 보간 */
        vResultPos = Vec3::Lerp(vStartCinematicPos, vEndCinematicPos, fReusltRatio);
        break;
    case ELerpType::SlowStart:                      break;
    case ELerpType::SlowEnd:                        break;
    default:                                        break;
    }

    /* Quat Ratio */
    switch (tEndCamKeyFrameData.eLookAtLerpType)
    {
    case ELerpType::NONE:
        vResultQuat = vEndCinematicQuat;   /* None 일때는 바로이동 */
        break;
    case ELerpType::Linear:
        fReusltRatio = fTimeRatio;
        vResultQuat = Quat::Slerp( vStartCinematicQuat , vEndCinematicQuat  , fReusltRatio);
        break;
    case ELerpType::SmoothStep:
        fReusltRatio = fTimeRatio * fTimeRatio * (3.0f - 2.0f * fTimeRatio);
        vResultQuat = Quat::Slerp(vStartCinematicQuat, vEndCinematicQuat, fReusltRatio);
        break;

    case ELerpType::SlowStart:                      break;
    case ELerpType::SlowEnd:                        break;
    default:                                        break;
    }


    pTs->Set_Info(TRANSFORM_INFO_STATE::POS , vResultPos);
    pTs->Rotation(vResultQuat);


    /* 총 도착시간 + HoldTime 시간 */
    _float fTotalFrameTime = tEndCamKeyFrameData.fDuration + tEndCamKeyFrameData.fHoldTime;

    /* 이 시간을 넘었다면 */
    if (m_fDeltaTime >= fTotalFrameTime)
    {
        m_iCurFrameIndex++;     // 다음 키프레임으로!
        m_fDeltaTime = 0.f;     // 시간 초기화


        if (m_iCurFrameIndex >= iSize)
        {
            /* 시네마틱 삭제 */
            m_isCinematicEvent  = false;
            m_fDeltaTime        = 0;
            m_iCurFrameIndex    = 0;
            m_pCinematicSquence = nullptr;
            m_pCinematicSquence->BroadCast(false);
        }
    }


    return;
}

void CCameraMan::Camera_Shaking(const CAM_SHAKING_DATA& tData)
{
    m_listCameraShakingDatas.push_back(tData);
    return;
}

void CCameraMan::Camera_Shaking(const _float fTimeDelta)
{
    m_listCameraShakingDatas.remove_if([](const CAM_SHAKING_DATA& data) {
        return data.fCurTime >= data.fTime;
        });

    if (m_listCameraShakingDatas.empty())
    {
        m_vCamShakingOffsetPosition = { Vec3::Zero };
        return;
    }


    m_vCamShakingOffsetPosition = { Vec3::Zero };

    /* CamShakingData */
    for (auto& CamShakingData : m_listCameraShakingDatas)
    {
        CamShakingData.fCurTime += fTimeDelta;
        _float fTimeRatio = CamShakingData.fCurTime / CamShakingData.fTime;

        /* 카메라 쉐이킹 강도를 시간에 따라 감소시켜줌 */
        _float strength = CamShakingData.fPower * (1.0f - fTimeRatio);

        _float randX = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        _float randY = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        _float randZ = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;

        m_vCamShakingOffsetPosition += Vec3(randX, randY, randZ) * strength;
    }

}

void CCameraMan::Cinematic(const Camera_Cinematic_Sequence* pCameraCinematicSequence)
{
    if (pCameraCinematicSequence == nullptr) return;
    
    /* Test */
    m_pCinematicSquence = pCameraCinematicSequence;
    m_isCinematicEvent = true;

    m_pCinematicSquence->BroadCast(true);
}

HRESULT CCameraMan::Ready_Components(void* pArg)
{
    return S_OK;
}

void CCameraMan::Free()
{
    Safe_Release(m_pActor);
    Super::Free();
}
