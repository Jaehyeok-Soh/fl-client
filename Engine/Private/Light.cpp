#include "Engine_pch.h"
#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_eType = LightDesc.eType;

    m_tLightDesc.vDiffuse = LightDesc.vDiffuse;
    m_tLightDesc.vAmbient = LightDesc.vAmbient;
    m_tLightDesc.vSpecular = LightDesc.vSpecular;

    m_tLightDesc.vDirection = LightDesc.vDirection;
    m_tLightDesc.vPosition = LightDesc.vPosition;
    m_tLightDesc.fRange = LightDesc.fRange;
    return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
    CLight* pInstance = new CLight();
    if (FAILED(pInstance->Initialize(LightDesc)))
    {
        MSG_BOX("CLight::Create, Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight::Free()
{
    Super::Free();
}
