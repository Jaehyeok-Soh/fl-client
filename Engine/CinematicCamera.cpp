#include "Engine_pch.h"
#include "CinematicCamera.h"

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice,pDeviceContext,CameraType::STATIC)
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& rhs)
    : Super(rhs)
{
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

HRESULT CCinematicCamera::Play_Cinematic(Camera_Cinematic_Sequence* pCamCinematicSequence)
{
    if (pCamCinematicSequence == nullptr)
    {
        MSG_BOX(" 받아온 시네마틱 데이터가 nullptr 입니다 데이터를 확인해주세요 ");
        return E_FAIL;
    }

    if (m_pCinematicSquence != nullptr)
    {
        MSG_BOX(" 현재 시네마틱 연출이 진행중입니다 데이터를 확인해주세요 ");
        return E_FAIL;
    }

    m_pCinematicSquence = pCamCinematicSequence;

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
}

void CCinematicCamera::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
    if (m_pCinematicSquence != nullptr)
        return;
}

void CCinematicCamera::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    if (m_pCinematicSquence != nullptr)
        return;


}

void CCinematicCamera::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    if (m_pCinematicSquence != nullptr)
        return;


}

HRESULT CCinematicCamera::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;


    return S_OK;
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
