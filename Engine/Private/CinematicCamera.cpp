#include "Engine_pch.h"
#include "CinematicCamera.h"
#include "GameInstance.h"

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice,pDeviceContext,CameraType::DYNAMIC)
    , m_pCinematicSquence{nullptr}
    , m_iCurFrameIndex{0}
    , m_iPreFrameIndex{NONE_INDEX}
    , m_fDeltaTime{0.f}
    , m_isActionStart{false}
    , m_vecCamer_KeyFrame_Data{}
    , m_isPreCamData_AddKeyFrame{false}
    , m_isDepratEvent{false}
    , m_isOnReachEvent{false}
{
    m_strName = Engine_Utils::ToString(g_wszCinematicCameraTag);
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& rhs)
    : Super(rhs)
    , m_pCinematicSquence{ nullptr }
    , m_iCurFrameIndex{ rhs.m_iCurFrameIndex }
    , m_iPreFrameIndex{ rhs.m_iPreFrameIndex }
    , m_fDeltaTime{ rhs.m_fDeltaTime }
    , m_isActionStart{ rhs.m_isActionStart}
    , m_vecCamer_KeyFrame_Data{rhs.m_vecCamer_KeyFrame_Data }
    , m_isPreCamData_AddKeyFrame{rhs.m_isPreCamData_AddKeyFrame }
    , m_isDepratEvent{ rhs.m_isDepratEvent }
    , m_isOnReachEvent{ rhs.m_isOnReachEvent }
{
    m_strName = Engine_Utils::ToString(g_wszCinematicCameraTag);
}

HRESULT CCinematicCamera::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CCinematicCamera::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    return S_OK;
}

HRESULT CCinematicCamera::Play_Cinematic(CinematicCameraSequence* pCamCinematicSequence)
{
    if (pCamCinematicSequence == nullptr)
    {
        MSG_BOX(" 받아온 시네마틱 데이터가 nullptr 입니다 데이터를 확인해주세요 ");
        return E_FAIL;
    }
    if (pCamCinematicSequence->vecCamKeyFrameDatas.empty())
    {
        MSG_BOX(" 카메라 키프레임이 존재하지않습니다 확인해주세요 ");
        return E_FAIL;
    }
    if (m_pCinematicSquence != nullptr)
    {
        MSG_BOX(" 현재 시네마틱 연출이 진행중입니다 데이터를 확인해주세요 ");
        return E_FAIL;
    }


    m_pCinematicSquence = pCamCinematicSequence;

    /* 0번을 검사해서 처음 카메라 시작지점을 지정해준다 */
    Camera_Keyframe_Data& tCamKeyFrameData = m_pCinematicSquence->vecCamKeyFrameDatas[0];
    Matrix ReusltMatrix{Matrix::Identity};
    if (tCamKeyFrameData.eMoveLerpType != ELerpType::NONE)
    {
        /* None이 아닐경우 0번자리에 기존 카메라 위치가 들어가게된다 */
        m_vecCamer_KeyFrame_Data.push_back(Camera_Keyframe_Data(m_pGameInstance->Get_MainCamera()));
        m_vecCamer_KeyFrame_Data.insert(m_vecCamer_KeyFrame_Data.end(), m_pCinematicSquence->vecCamKeyFrameDatas.begin(), m_pCinematicSquence->vecCamKeyFrameDatas.end());
        /* 현재 Main 카메라의위치를 가져와준다. */
        ReusltMatrix = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_WorldMatrix();
        m_isPreCamData_AddKeyFrame = true;
    }
    else
    {
        /* None일떄 0 번자리로 바로 순간이동 해준다 */
        m_vecCamer_KeyFrame_Data.assign(m_pCinematicSquence->vecCamKeyFrameDatas.begin(), m_pCinematicSquence->vecCamKeyFrameDatas.end());
        ReusltMatrix = tCamKeyFrameData.Get_WorldMatrix();
    }

    this->Get_Component<CTransform>()->Set_WorldMatrix(ReusltMatrix);

    m_pGameInstance->Change_MainCamera(CameraType::DYNAMIC,g_wszCinematicCameraTag);

    return S_OK;
}


HRESULT CCinematicCamera::Awake(const _uint iCurrentLevelID)
{
    if(FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CCinematicCamera::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
    if (!m_pCinematicSquence) return;


    /* Cinematic이 시작되면 한번만 불리는 시작함수 */
    if (m_isActionStart)
    {
        m_pCinematicSquence->BroadCast_BeginEvent();
        m_isActionStart = false;
    }

}

void CCinematicCamera::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    if (m_pCinematicSquence == nullptr)
        return;

    m_fDeltaTime += fTimeDelta;
    

    CCamera* pCameraCom = Get_Component<CCamera>(); if (pCameraCom == nullptr) return;
    CTransform* pTs = Get_Component<CTransform>();  if (pTs == nullptr) return;

    /* 현재 키프레임 데이터 */
    _int iSize = static_cast<_int>(m_vecCamer_KeyFrame_Data.size());

    if(iSize < 2 )
        End_Cinematic();

    _int iOriginalStartIndex = m_isPreCamData_AddKeyFrame ? (m_iCurFrameIndex - 1) : m_iCurFrameIndex;
    _int iOriginalEndIndex = m_isPreCamData_AddKeyFrame ? (m_iCurFrameIndex) : (m_iCurFrameIndex + 1);

    auto& tStartCamKeyFrameData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex];
    auto& tEndCamKeyFrameData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex + 1];

    if (!m_isDepratEvent)
    {
        // Dummy 프레임(-1)에서 출발하는 게 아닐 때만 발송!
        if (iOriginalStartIndex >= 0)
        {
            m_pCinematicSquence->BroadCast(CCS_BROADCAST_TYPE::DEPART, iOriginalStartIndex);
        }
        m_isDepratEvent = true; // ★ 이거 빼먹으면 매 프레임 발송되니까 조심!
    }


    /* World 가져와서 적용 */
    Matrix  CurCamWorldMatrix = pTs->Get_WorldMatrix();
    Vec3    vCurCamScale{}, vCurCamPos{};
    Quat    vCurCamQuat{};
    CurCamWorldMatrix.Decompose(vCurCamScale, vCurCamQuat, vCurCamPos);

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
    {
        vResultPos = vEndCinematicPos;   /* None 일때는 바로이동 */
        break;
    }
    vResultPos = vEndCinematicPos;   /* None 일때는 바로이동 */
    break;
    case ELerpType::Linear:             /* 단순 이동 */
    {

        fReusltRatio = fTimeRatio;
        vResultPos = Vec3::Lerp(vStartCinematicPos, vEndCinematicPos, fReusltRatio);
        break;
    }
    case ELerpType::SmoothStep:
    {
        fReusltRatio = fTimeRatio * fTimeRatio * (3.0f - 2.0f * fTimeRatio);          /* 스무스 보간 */
        vResultPos = Vec3::Lerp(vStartCinematicPos, vEndCinematicPos, fReusltRatio);
        break;
    }
    case ELerpType::SlowStart:                      break;
    case ELerpType::SlowEnd:                        break;
    case ELerpType::Curve:
    {
        fReusltRatio = fTimeRatio;

        /* Pre Pos */
        Vec3 vPrevPos = vStartCinematicPos; // 인덱스가 없으면 그냥 Start 위치를 쓴다!
        if (m_iCurFrameIndex > 0)
        {
            auto& tPreData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex - 1];
            vPrevPos = tPreData.Get_WorldMatrix().Translation();
        }

        /* 다다음 점구하기 */
        Vec3 vNextPos = vEndCinematicPos; //만약없다면 NexpPos => 는 내가 이동할 Pos그대로사용
        if (m_iCurFrameIndex + 2 < iSize)
        {
            auto& tNextData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex + 2];
            vNextPos = tNextData.Get_WorldMatrix().Translation();
        }
        vResultPos = Vec3::CatmullRom(vPrevPos, vStartCinematicPos, vEndCinematicPos, vNextPos, fReusltRatio);
        break;
    }
    default:                                        break;
    }

    /* Quat Ratio */
    switch (tEndCamKeyFrameData.eLookAtLerpType)
    {
    case ELerpType::NONE:
    {
        vResultQuat = vEndCinematicQuat;   /* None 일때는 바로이동 */
        break;
    }
    case ELerpType::Linear:
    {
        fReusltRatio = fTimeRatio;
        vResultQuat = Quat::Slerp(vStartCinematicQuat, vEndCinematicQuat, fReusltRatio);
        break;
    }
    case ELerpType::SmoothStep:
    {
        fReusltRatio = fTimeRatio * fTimeRatio * (3.0f - 2.0f * fTimeRatio);
        vResultQuat = Quat::Slerp(vStartCinematicQuat, vEndCinematicQuat, fReusltRatio);
        break;
    }

    case ELerpType::SlowStart:                      break;
    case ELerpType::SlowEnd:                        break;

    case ELerpType::Curve:
    {
        fReusltRatio = fTimeRatio;

        Quat vPrevQuat = vStartCinematicQuat;       // 이동과 마찬가지로 사용
        if (m_iCurFrameIndex > 0)
        {
            auto& tPreData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex - 1];
            tPreData.Get_WorldMatrix().Decompose(vCurCamScale, vPrevQuat, vCurCamPos);
        }

        Quat vNextQuat = vEndCinematicQuat;
        if (m_iCurFrameIndex + 2 < iSize)
        {
            auto& tNextData = m_vecCamer_KeyFrame_Data[m_iCurFrameIndex + 2];
            tNextData.Get_WorldMatrix().Decompose(vCurCamScale, vNextQuat, vCurCamPos);
        }

        XMVECTOR A, B, C;

        // Setup 함수로 곡선 제어점(A, B, C) 생성
        XMQuaternionSquadSetup(&A, &B, &C, vPrevQuat, vStartCinematicQuat, vEndCinematicQuat, vNextQuat);

        /* 3. 보간 결과도 SimpleMath::Quaternion이 XMVECTOR를 바로 받을 수 있습니다. */
        vResultQuat = XMQuaternionSquad(vStartCinematicQuat, A, B, vEndCinematicQuat, fReusltRatio);

        break;
    }
    default:                                        break;
    }


    pTs->Set_Info(TRANSFORM_INFO_STATE::POS, vResultPos);
    pTs->Rotation(vResultQuat);


    /* 총 도착시간 + HoldTime 시간 */
    _float fTotalFrameTime = tEndCamKeyFrameData.fDuration + tEndCamKeyFrameData.fHoldTime;

    /* 도착했을떄 Event발송 */
    if (m_fDeltaTime >= tEndCamKeyFrameData.fDuration)
    {
        if (!m_isOnReachEvent)
        {
            // 원본 데이터 범위를 벗어나지 않는 안전한 인덱스일 때만 발송
            _int iOriginalMaxCount = static_cast<_int>(m_pCinematicSquence->vecCamKeyFrameDatas.size());
            if (iOriginalEndIndex >= 0 && iOriginalEndIndex < iOriginalMaxCount)
            {
                m_pCinematicSquence->BroadCast(CCS_BROADCAST_TYPE::ON_REACH, iOriginalEndIndex);
            }
            m_isOnReachEvent = true; // ★ 필수!
        }
    }

    /* 이 시간을 넘었다면 */
    if (m_fDeltaTime >= fTotalFrameTime)
    {

        m_isDepratEvent = false;
        m_isOnReachEvent = false;
        m_iPreFrameIndex = m_iCurFrameIndex; /* 이건 KeyFrame 들고있기 */
        m_iCurFrameIndex++;     // 다음 키프레임으로!
        m_fDeltaTime = 0.f;     // 시간 초기화

        if (m_iCurFrameIndex + 1 >= iSize)
        {
            End_Cinematic();
        }
    }
}

void CCinematicCamera::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    if (m_pCinematicSquence == nullptr)
        return;
}

void CCinematicCamera::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    if (m_pCinematicSquence == nullptr)
        return;
}

HRESULT CCinematicCamera::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}



HRESULT CCinematicCamera::End_Cinematic()
{
    /* 시네마틱 삭제 */
    m_isCinematicEvent = false;
    m_isPreCamData_AddKeyFrame = false;

    m_fDeltaTime = 0;
    m_iCurFrameIndex = 0;
    m_pCinematicSquence->BroadCast(CCS_BROADCAST_TYPE::END_CCS);
    m_pCinematicSquence = nullptr;

    m_vecCamer_KeyFrame_Data.clear();

    m_pGameInstance->End_CameraCinematic();

    return S_OK;
}

void CCinematicCamera::Update_PreMove(const _float fTimeDelta)
{
    




    return;
}


CCinematicCamera* CCinematicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CCinematicCamera* pCinematicCamera = new CCinematicCamera(pDevice, pDeviceContext);

    if (FAILED(pCinematicCamera->Initialize_Prototype()))
    {
        Safe_Release(pCinematicCamera);
        MSG_BOX(" Cinematic Camera is Failed to Create ");
        return nullptr;
    }
    return pCinematicCamera;
}

CGameObject* CCinematicCamera::Clone(void* pArg)
{
    CCinematicCamera* pCinematicCamera = new CCinematicCamera(*this);

    if (FAILED(pCinematicCamera->Initialize(pArg)))
    {
        Safe_Release(pCinematicCamera);
        MSG_BOX("Cinematic Camera is Failed to Clone");
        return nullptr;
    }
    return pCinematicCamera;
}

void CCinematicCamera::Free()
{
    Super::Free();
    m_pCinematicSquence = nullptr;
}
