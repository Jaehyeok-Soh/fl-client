#include "Engine_pch.h"
#include "CameraMan.h"
#include "GameInstance.h"

CCameraMan::CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType)
    : Super(pDevice, pDeviceContext)
    , m_eType(eType)
    , m_vCamShakingOffsetPosition{Vec3::Zero}
{
}

CCameraMan::CCameraMan(const CCameraMan& rhs)
    : Super(rhs)
    , m_eType(rhs.m_eType)
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

    if (FAILED(Add_Component<CCamera>(0/* static */, L"Prototype_Component_Camera", pDesc->pCamera_Desc)))
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




    Camera_Shaking(fTimeDelta);
}

void CCameraMan::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
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

HRESULT CCameraMan::Ready_Components(void* pArg)
{
    return S_OK;
}

void CCameraMan::Free()
{
    Safe_Release(m_pActor);
    Super::Free();
}
