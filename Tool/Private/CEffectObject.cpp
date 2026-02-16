#include "pch.h"
#include "CEffectObject.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Model.h"
#include "Texture.h"
#include "mesh.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Mesh.h"
#include "StructuredBuffer.h"
#include "GameInstance.h"

#define PLAY 0
#define PAUSE 1
#define RESET 2
#define STOP 3

CEffectObject::CEffectObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :Tool_PartObject(eType, pDevice, pDeviceContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
    :Tool_PartObject(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    Effect_Desc* pDesc = static_cast<Effect_Desc*>(pArg);
    if (pDesc != nullptr)
        m_tEffectDesc = *pDesc;

    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::Initialize(pArg)");
        return E_FAIL;
    }

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectObject::Ready_Component(void* pArg)
{
    if (FAILED(Ready_Component_Shader()))
        return E_FAIL;

    if (FAILED(Ready_Component_Texture()))
        return E_FAIL;

    if (FAILED(Ready_Component_Model(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component_Buffer(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Shader()
{
    // ========   Rendering Shader   ========
    {
        CShader::SHADER_ORIGIN_DESC ShaderDesc = {};
        wstring ShaderTag = L"Prototype_Component_";
        Add_Component<CShader>(0, ShaderTag + m_tEffectDesc._Effect_Shader_Tag, &ShaderDesc);
        Get_Component<CShader>()->Set_Pass(m_tEffectDesc._Effect_ShaderPass);
    }

    // Compute Shader 세팅
    {
        CComputeShader::ComShaderCopyDesc ShaderDesc = {};
        ShaderDesc.Output_SRVBuffer_Name = "INSTANCE_RESULT_SRV";
        // 입력 버퍼
        ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
        ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
        ShaderDesc.Input_StructBuffer.iNumElements = m_tEffectDesc._Effect_MaxParticle;
        ShaderDesc.InputBufferNum = 1;

        // 출력 버퍼
        ShaderDesc.OutPut_StructBuffer.sBufferName = "INSTANCE_OUTPUT";
        ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(EFFECT_INSTANCE);
        ShaderDesc.OutPut_StructBuffer.iNumElements = m_tEffectDesc._Effect_MaxParticle;

        if (FAILED(Add_Script_Component(L"ComputeShader", L"Prototype_Component_Shader_CPT_Effect_Particle", &ShaderDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Texture()
{
    CTexture::TEXTURE_COMPONENT_ORIGIN_DESC desc = {};
    wstring s = L"Texture_";

    // 빈 깡통 텍스처로 교체해주고.

    if (Get_Component<CTexture>())
    {
        Change_Component<CTexture>(static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Texture_Empty", &desc)));

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GRADATION));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DissolveTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DISSOLVE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GlowTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GLOW));
        }
    }

    else
    {
        Add_Component<CTexture>(0, L"Prototype_Component_Texture_Empty", &desc);

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GRADATION));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DissolveTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DISSOLVE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GlowTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GLOW));
        }
    }

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Model(void* pArg)
{
    CModel::MODEL_COPY_DESC pModelDesc = {};
    wstring s = L"Prototype_Component_Model_";

    if (m_tEffectDesc.eEffectParticleType == E_PARTICLETYPE::MESH)
    {
        if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::EFFECT), s + m_tEffectDesc._Effect_Model_Tag, &pModelDesc)))
            return E_FAIL;
    } 

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Buffer(void* pArg)
{
    Effect_Desc* pDesc = static_cast<Effect_Desc*>(pArg);

    switch (m_tEffectDesc.eEffectParticleType)
    {
    case E_PARTICLETYPE::PARTICLE:
    {
        CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
        pParticleDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
        pParticleDesc.isLoop = m_tEffectDesc._Effect_Looping;
        pParticleDesc.vLifeTime.x = 0;
        pParticleDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
        pParticleDesc.vRange = m_tEffectDesc._Effect_Range;
        pParticleDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
        pParticleDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
        pParticleDesc.vSpeed = Vec2{ 1.f, 2.f };
        pParticleDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
        pParticleDesc.pOwner = this;
        pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
        pParticleDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

        if (FAILED(Add_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)))))
            return E_FAIL;
        break;
    }
    case E_PARTICLETYPE::MESH:
    {
        // 모델이 있을 때.
        if (Get_Component<CModel>())
        {
            CModel* pInstance = Get_Component<CModel>();
            CVIBuffer_Particle_Mesh::PARTICLE_Mesh_ORIGIN_DESC MeshBufferDesc = {};
            MeshBufferDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
            MeshBufferDesc.isLoop = m_tEffectDesc._Effect_Looping;
            MeshBufferDesc.vLifeTime.x = MeshBufferDesc.vLifeTime.x;
            MeshBufferDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
            MeshBufferDesc.vRange = m_tEffectDesc._Effect_Range;
            MeshBufferDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
            MeshBufferDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
            MeshBufferDesc.vSpeed = Vec2{ 1.f, 2.f };
            MeshBufferDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
            MeshBufferDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
            MeshBufferDesc.pModel = pInstance;
            MeshBufferDesc.pOwner = this;
            MeshBufferDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

            if (FAILED(Add_Component<CVIBuffer_Particle_Mesh>(static_cast<CVIBuffer_Particle_Mesh*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Mesh", &MeshBufferDesc)))))
                return E_FAIL;
        }
        break;
    }
    case E_PARTICLETYPE::TEXTURE:
    {
        CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
        pParticleDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
        pParticleDesc.isLoop = m_tEffectDesc._Effect_Looping;
        pParticleDesc.vLifeTime.x = pParticleDesc.vLifeTime.x;
        pParticleDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
        pParticleDesc.vRange = m_tEffectDesc._Effect_Range;
        pParticleDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
        pParticleDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
        pParticleDesc.vSpeed = Vec2{ 1.f, 2.f };
        pParticleDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
        pParticleDesc.pOwner = this;
        pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
        pParticleDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

        if (FAILED(Add_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)))))
            return E_FAIL;
    }
    }

    return S_OK;
}

void CEffectObject::Set_EffectDesc(const Effect_Desc& Desc)
{
    m_tEffectDesc = Desc;
    TimeFlagRequest(RESET);

    Model_Setting(m_tEffectDesc._Effect_Model_Tag);
    Texture_Setting(m_tEffectDesc._Effect_DiffuseTexture_Tag);
    Shader_Setting(m_tEffectDesc._Effect_Shader_Tag);
    Buffer_Setting();
    Particle_Setting();

    m_tPrevEffectDesc = m_tEffectDesc;
}

void CEffectObject::Buffer_Setting()
{
    if (m_tPrevEffectDesc._Effect_Model_Tag != m_tEffectDesc._Effect_Model_Tag || m_tPrevEffectDesc.eEffectParticleType != m_tEffectDesc.eEffectParticleType)
    {
        switch (m_tEffectDesc.eEffectParticleType)
        {
        case E_PARTICLETYPE::PARTICLE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
            pParticleDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
            pParticleDesc.isLoop = m_tEffectDesc._Effect_Looping;
            pParticleDesc.vLifeTime.x = pParticleDesc.vLifeTime.x;
            pParticleDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
            pParticleDesc.vRange = m_tEffectDesc._Effect_Range;
            pParticleDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
            pParticleDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
            pParticleDesc.vSpeed = Vec2{ 0.f, 3.f };
            pParticleDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
            pParticleDesc.pOwner = this;
            pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
            pParticleDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

            Change_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)));
            break;
        }
        case E_PARTICLETYPE::MESH:
        {
            // 모델이 있을 때.
            if (Get_Component<CModel>())
            {
                CModel* pInstance = Get_Component<CModel>();
                CVIBuffer_Particle_Mesh::PARTICLE_Mesh_ORIGIN_DESC MeshBufferDesc = {};
                MeshBufferDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
                MeshBufferDesc.isLoop = m_tEffectDesc._Effect_Looping;
                MeshBufferDesc.vLifeTime.x = MeshBufferDesc.vLifeTime.x;
                MeshBufferDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
                MeshBufferDesc.vRange = m_tEffectDesc._Effect_Range;
                MeshBufferDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
                MeshBufferDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
                MeshBufferDesc.vSpeed = Vec2{ 0.f, 3.f };
                MeshBufferDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
                MeshBufferDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
                MeshBufferDesc.pModel = pInstance;
                MeshBufferDesc.pOwner = this;
                MeshBufferDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

                Change_Component<CVIBuffer_Particle_Mesh>(static_cast<CVIBuffer_Particle_Mesh*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Mesh", &MeshBufferDesc)));
            }
            break;
        }
        case E_PARTICLETYPE::TEXTURE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
            pParticleDesc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
            pParticleDesc.isLoop = m_tEffectDesc._Effect_Looping;
            pParticleDesc.vLifeTime.x = pParticleDesc.vLifeTime.x;
            pParticleDesc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
            pParticleDesc.vRange = m_tEffectDesc._Effect_Range;
            pParticleDesc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
            pParticleDesc.vSize = m_tEffectDesc._Effect_ParticleSize;
            pParticleDesc.vSpeed = Vec2{ 0.f, 3.f };
            pParticleDesc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
            pParticleDesc.pOwner = this;
            pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
            pParticleDesc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

            Change_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)));
            break;
        }
        }
    }
}

void CEffectObject::Particle_Setting()
{
    if (m_tEffectDesc.eEffectType == E_EFFECTTYPE::Particle)
    {
        CVIBuffer_Particle* pInstance = nullptr;
        if(m_tEffectDesc.eEffectParticleType != E_PARTICLETYPE::MESH)
            pInstance = Get_Component<CVIBuffer_Particle_Point>();
        else
            pInstance = Get_Component<CVIBuffer_Particle_Mesh>();

        auto pDesc = pInstance->Get_ParticleDesc();
        CVIBuffer_Particle::PARTICLE_ORIGIN_DESC desc = {};

        desc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
        desc.isLoop = m_tEffectDesc._Effect_Looping;
        desc.vLifeTime.x = pDesc.vLifeTime.x;
        desc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
        desc.vRange = m_tEffectDesc._Effect_Range;
        desc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
        desc.vSize = m_tEffectDesc._Effect_ParticleSize;
        desc.vSpeed = Vec2{ 0.f, 3.f };
        desc.isRandomSeed = ENUM_TO_UINT(m_tEffectDesc._Effect_EmissionType);
        desc.pModel = Get_Component<CModel>();
        desc.pOwner = this;
        desc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
        desc.EmissionFlagType = (_uint)m_tEffectDesc._Effect_EmissionType;

        pInstance->Set_ParticleDesc(desc);
    }
}

void CEffectObject::Model_Setting(const wstring& ModelName)
{
    if (m_tPrevEffectDesc._Effect_Model_Tag != m_tEffectDesc._Effect_Model_Tag)
    {
        CModel::MODEL_COPY_DESC pDesc = {};
        wstring s = L"Prototype_Component_Model_";

        if (Get_Component<CModel>())
            Change_Component<CModel>(static_cast<CModel*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::EFFECT), s + ModelName, &pDesc)));

        else
            if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::EFFECT), s + ModelName, &pDesc)))
                return;
    }
}

void CEffectObject::Texture_Setting(const wstring& TextureName)
{
    CTexture::TEXTURE_COMPONENT_ORIGIN_DESC desc = {};
    wstring s = L"Texture_";

    // 빈 깡통 텍스처로 교체해주고.

    if (Get_Component<CTexture>())
    {
        Change_Component<CTexture>(static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Texture_Empty", &desc)));

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GRADATION));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DissolveTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DISSOLVE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GlowTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GLOW));
        }
    }

    else
    {
        Add_Component<CTexture>(0, L"Prototype_Component_Texture_Empty", &desc);

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GRADATION));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DissolveTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::DISSOLVE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GlowTexture_Tag, ENUM_TO_UINT(TEXTURETYPE::GLOW));
        }
    }

}

void CEffectObject::Shader_Setting(const wstring& ShaderName)
{
    CShader::SHADER_ORIGIN_DESC ShaderDesc = {};

    if (Get_Component<CShader>())
    {
        if (ShaderName == L"Shader_VtxEffectParticle")
            Change_Component<CShader>(static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Shader_VtxEffectParticle", &ShaderDesc)));

        else if (ShaderName == L"Shader_VtxEffectMesh")
            Change_Component<CShader>(static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Shader_VtxEffectMesh", &ShaderDesc)));

        else if (ShaderName == L"Shader_VtxEffectTexture")
            Change_Component<CShader>(static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Shader_VtxEffectTexture", &ShaderDesc)));
    }

    else
    {
        if (ShaderName == L"Shader_VtxEffectParticle")
            Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxEffectParticle", &ShaderDesc);

        else if (ShaderName == L"Shader_VtxEffectMesh")
            Change_Component<CShader>(static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_Shader_VtxEffectMesh", &ShaderDesc)));

        else if (ShaderName == L"Shader_VtxEffectTexture")
            Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxEffectTexture", &ShaderDesc);
    }
}

HRESULT CEffectObject::Bind_ShaderResource()
{
    CShader* pShader = Get_Component<CShader>();
    CModel* pModel = Get_Component<CModel>();

    if (pShader == nullptr) return S_OK;
    pShader->Set_Pass(m_tEffectDesc._Effect_ShaderPass);
    pShader->Bind_TransformData(m_CombineWorldMatrix);

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Scene, pShader)))
        return E_FAIL;

    // 셰이더에 던질 구조체 작성하기.
    {
        SHADER_EFFECT_DESC pDesc = {};
        // TextureFlag
        pDesc.iTextureFlags = m_tEffectDesc._Effect_TextureFlag;
        // RenderFlag
        pDesc.iRenderFlags = m_tEffectDesc._Effect_RenderFlag;
        // SamplerStateFlag
        pDesc.iSamplerStateFlags = m_tEffectDesc._Effect_SamplerStateFlag;
        // DiscardValue
        pDesc.iDiscardValue = m_tEffectDesc._Effect_DiscardValue;
        // Texture_RotationFlag
        pDesc.iRotationFlags = m_tEffectDesc._Effect_TextureRotationFlag;
        // Texture_OperatorFlag
        pDesc.iOperatorFlags = m_tEffectDesc._Effect_TextureOperatorFlag;
        pDesc.vUVOffset = m_tEffectDesc._Effect_UV_Offset;

        pDesc.SpriteColCount = m_tEffectDesc._Effect_TileCount.x;
        pDesc.SpriteRowCount = m_tEffectDesc._Effect_TileCount.y;
        pDesc.CurSpriteIndex = m_tEffectDesc.m_iCurSpriteNumber;
        pDesc.LifeRatio = m_tEffectDesc._Effect_ApearRatio;

        pDesc.vDistortionScale = m_tEffectDesc._Effect_DistortionScale;
        pDesc.vEffectColor = m_tEffectDesc._Effect_Color;
        pDesc.vScrollOffset = m_vScrollOffset;

        pDesc.DiffuseTexture_ScrollWeight = m_tEffectDesc._Effect_DiffuseTexture_ScrollWeight;
        pDesc.NoiseTexture_ScrollWeight = m_tEffectDesc._Effect_NoiseTexture_ScrollWeight;
        pDesc.MaskingTexture_ScrollWeight = m_tEffectDesc._Effect_MaskingTexture_ScrollWeight;
        pDesc.GradationTexture_ScrollWeight = m_tEffectDesc._Effect_GradationTexture_ScrollWeight;

        pShader->Bind_EffectData(pDesc);

        // Compute 셰이더가 들고있는 SRV를, Default Shader한테 SRV 꽂아주기.
        {
            ID3D11ShaderResourceView* pResultSRV = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"))->Get_Output_Buffer()->Get_SRV();

            ID3DX11EffectShaderResourceVariable* pSRVar = Get_Component<CShader>()->Get_SRV("INSTANCE_OUTPUT");
            if (pSRVar)
                pSRVar->SetResource(pResultSRV);
        }
    }

    // 텍스처 바인딩
    {
        CTexture* pTexture = Get_Component<CTexture>();
        if (pTexture)
            pTexture->Bind_ShaderResourceBuffer(Get_Component<CShader>());
    }

    if (m_tEffectDesc.eEffectParticleType != E_PARTICLETYPE::MESH || !Get_Component<CModel>())
    {
        CVIBuffer_Particle_Point* pInstance = static_cast<CVIBuffer_Particle_Point*>(Get_Component<CVIBuffer_Particle_Point>());

        if (pInstance)
        {
            pInstance->Bind_Resource();
            pShader->Apply();
            pInstance->Render();
        }
    }

    else if (m_tEffectDesc.eEffectParticleType == E_PARTICLETYPE::MESH && Get_Component<CModel>())
    {
        CVIBuffer_Particle_Mesh* pInstance = static_cast<CVIBuffer_Particle_Mesh*>(Get_Component<CVIBuffer_Particle_Mesh>());

        if (pInstance)
        {
            pInstance->Bind_Resource();
            pShader->Apply();
            pInstance->Render();
        }
    }

    return S_OK;
}

HRESULT CEffectObject::Awake(const _uint iCurrentLevelID)
{
    Super::Awake(iCurrentLevelID);

    return S_OK;
}

void CEffectObject::Update_Priority(const _float fDT)
{
    // 임시 방편 Speed
    Super::Update_Priority(fDT * m_tEffectDesc._Effect_PlayBackSpeed);
}

void CEffectObject::Update(const _float fTimeDelta)
{
    float TimeFlag = 0.f;

    switch (m_tEffectDesc._Effect_TimeFlag)
    {
    case PLAY: TimeFlag = 1.f; break;
    case PAUSE:TimeFlag = 0.f; break;
    case RESET: TimeFlag = 1.f; break;
    case STOP: TimeFlag = 0.f; break;
    }

    _float TimeT = m_tEffectDesc._Effect_PlayBackSpeed * fTimeDelta * TimeFlag;
    m_fTimeAccumulation += TimeT; // 전체 시간 누적

    //  ========   Start Delay 체크   ========   
    if (m_fTimeAccumulation < m_tEffectDesc._Effect_StartDelay)
    {
        m_bIsStarted = false;
        return; // 아직 대기 중이므로 업데이트 안 함
    }
    m_bIsStarted = true;

    //========   실제 시뮬레이션 시간 (Delay를 제외한 시간)  ========   
    _float fActiveTime = m_fTimeAccumulation - m_tEffectDesc._Effect_StartDelay;

    // ========   Duration 및 Looping 제어   ========   
    if (fActiveTime >= m_tEffectDesc._Effect_Duration)
    {
        if (m_tEffectDesc._Effect_Looping)
        {
            m_fTimeAccumulation = 0.f; // 완전 리셋
            fActiveTime = 0.f;
            m_vScrollOffset = { 0.f, 0.f }; // 스크롤 값도 완전 초기화

            // 루프 시 파티클 버퍼 리셋이 필요하다면 호출
            auto pVIBuffer = Get_Component<CVIBuffer_Particle_Point>();
            if (pVIBuffer) pVIBuffer->Reset_Simulation();
        }
        else
        {
            return;
        }
    }

    // ========   크기 보간 (Start Size -> End Size)   ========   
    // 진행률 계산 (0.0 ~ 1.0)
    _float fRatio = fActiveTime / m_tEffectDesc._Effect_Duration;
    if (fRatio > 1.f) fRatio = 1.f;

    // 선형 보간을 통한 실시간 스케일 계산
    Vec3 vCurrentScale = Vec3::Lerp(m_tEffectDesc._Effect_StartScale, m_tEffectDesc._Effect_EndScale, fRatio);
    Get_Component<CTransform>()->Set_Scale(vCurrentScale);

    TimeCalculate(TimeT);
    Update_UV_Scroll_Curve(fRatio);
    Update_Rotation_Lerp(TimeT, fRatio);
    Super::Update(TimeT);
    // == 스크롤 값 == 

    auto CTShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
    switch (m_tEffectDesc._Effect_ShapeType)
    {
    case E_SHAPETYPE::NONE:
        break;

    case E_SHAPETYPE::SPREAD:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::SPREAD);
        break;
    }
    case E_SHAPETYPE::DROP:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::DROP);
        break;
    }
    case E_SHAPETYPE::RISE:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::RISE);
        break;
    }
    case E_SHAPETYPE::SPIRAL:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::SPIRAL);
        break;
    }

    case E_SHAPETYPE::DNA:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::DNA);
        break;
    }

    case E_SHAPETYPE::STRAIGHT:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK), TimeT, m_tEffectDesc._Effect_TimeFlag, E_PARTICLE_MOVESTATE::STRAIGHT);
        break;
    }
    }
}

void CEffectObject::Update_Late(const _float fTimeDelta)
{
    _float TimeT = m_tEffectDesc._Effect_PlayBackSpeed * fTimeDelta;

    Super::Update_Late(TimeT);
}

void CEffectObject::Ready_Before_Render(const _float fTimeDelta)
{
    // 임시 방편 
    _float TimeT = m_tEffectDesc._Effect_PlayBackSpeed * fTimeDelta;

    Super::Ready_Before_Render(TimeT);

    if (m_tEffectDesc._Effect_ShaderPass != 3)
        m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONELIGHT, this);

    else
        m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::DISTOTION, this);

    Super::Update_CombinedWorldMatrix(m_pMatParent);
}

// 
void CEffectObject::Preview_TextureKey_Binding(const string& Key)
{

}

void CEffectObject::Preview_Texture_Reset()
{

}

bool CEffectObject::IntsersectWithPlane(OUT Vec3& vOut)
{
    //CModel* pModel = Get_Component<CModel>();
    //if (pModel == nullptr)  return false;

    //_uint iMeshCount = pModel->Get_MeshCount();
    //for (_uint i = 0; i < iMeshCount; ++i)
    //{
    //    if (pModel->Get_Mesh(i)->IntsersectWithPlane(vOut))
    //        return true;
    //}
    return false;
}


HRESULT CEffectObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    // ===========  셰이더에 값 바인딩  ===========
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    return S_OK;
}

_bool CEffectObject::Picking(OUT Vec3& vOut)
{
    //const Matrix& matWorld = Get_Component<CTransform>()->Get_WorldMatrix();
    //Matrix matLocal = {};

    //matLocal = matWorld.Invert();
    //m_pGameInstance->TransformRayToLocalSpace(matLocal);

    //if (IntsersectWithPlane(vOut))
    //{
    //    vOut = Vec3::Transform(vOut, matWorld);
    //    return true;
    //}
    return false;
}

_bool CEffectObject::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    return false;
}

void CEffectObject::Draw_ImGui()
{
    Super::Draw_ImGui();
}

void CEffectObject::Set_Dead(const wstring& wstrLayerTag)
{
    Super::Set_Dead(wstrLayerTag);
}


void CEffectObject::TimeFlagRequest(_uint iTimeFlag)
{
    m_tEffectDesc._Effect_TimeFlag = iTimeFlag;

    if (iTimeFlag == RESET)
    {
        m_bIsStarted = false;
        m_fTimeAccumulation = 0.f;
        m_vScrollOffset = Vec2{ 0.f, 0.f };
        m_vAccumulatedRotation = { 0.f, 0.f, 0.f };
        m_vScrollOffset = { 0.f, 0.f }; // 스크롤 값도 완전 초기화

        auto CTShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance && CTShader)
            pInstance->Update_Simulation(CTShader, Vec3{}, 0.f, RESET, (E_PARTICLE_MOVESTATE)m_tEffectDesc._Effect_ShapeType);

        // 리셋 직후 바로 PLAY 상태로 전이시켜서 셰이더가 다음 루프를 돌게 함
        m_tEffectDesc._Effect_TimeFlag = PLAY;
        Set_Visible();
    }
    else if (iTimeFlag == STOP)
    {
        m_bIsStarted = false;
        m_fTimeAccumulation = 0.f;
        m_vScrollOffset = Vec2{ 0.f, 0.f };
        m_vAccumulatedRotation = { 0.f, 0.f, 0.f }; 

        auto CTShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance && CTShader)
            pInstance->Update_Simulation(CTShader, Vec3{}, 0.f, STOP, (E_PARTICLE_MOVESTATE)m_tEffectDesc._Effect_ShapeType);

        m_fTimeAccumulation = 0.f;
        Set_Invisible();
        // Stop은 PLAY로 바꾸지 않고 그대로 둠
    }

    else
        Set_Visible();
}

void CEffectObject::TimeCalculate(const _float fDT)
{
    // Start Delay를 제외한 순수 실행 시간 기반 진행률
    _float fActiveTime = m_fTimeAccumulation - m_tEffectDesc._Effect_StartDelay;
    if (fActiveTime < 0.f) fActiveTime = 0.f;

    // ======= [스프라이트 애니메이션] =======
    if ((m_tEffectDesc._Effect_RenderFlag & (1 << 5)) && m_tEffectDesc._Effect_bPlayAnim)
    {
        _uint iTotalFrame = m_tEffectDesc._Effect_TileCount.x * m_tEffectDesc._Effect_TileCount.y;
        if (iTotalFrame > 0)
        {
            _uint iFrame = (_uint)(fActiveTime * m_tEffectDesc._Effect_AnimSpeed);

            if (m_tEffectDesc._Effect_Looping)
                m_tEffectDesc.m_iCurSpriteNumber = iFrame % iTotalFrame;
            else
                m_tEffectDesc.m_iCurSpriteNumber = std::min(iFrame, iTotalFrame - 1);
        }
    }
}

float CEffectObject::Sample_GravityCurve(const vector<Gravity_CurveKey>& vecCurve, float fLifeRatio)
{
    // 키 프레임 데이터가 없다면 기본 1.0f 반환
    if (vecCurve.empty())
        return 1.0f;

    // 데이터가 만약에 하나라면 그놈만 반환하기.
    if (vecCurve.size() == 1)
        return vecCurve[0].fValue;

    // 비율이 범위를 벗어나면 처음이나 끝으로 clamp하기.
    if (fLifeRatio <= vecCurve.front().fTimeKey) return vecCurve.front().fValue;
    if (fLifeRatio >= vecCurve.back().fTimeKey) return vecCurve.back().fValue;

    // 현재 비율이 위치한 key frame 찾기.
    for (size_t i = 0; i < vecCurve.size() - 1; ++i)
    {
        if (fLifeRatio >= vecCurve[i].fTimeKey && fLifeRatio <= vecCurve[i + 1].fTimeKey)
        {
            // 두 키 프레임 사이의 진행도 계산
            float fDeltaTime = vecCurve[i + 1].fTimeKey - vecCurve[i].fTimeKey;
            float fLerpRatio = (fLifeRatio - vecCurve[i].fTimeKey) / fDeltaTime;

            // 선형보간
            return vecCurve[i].fValue + (vecCurve[i + 1].fValue - vecCurve[i].fValue) * fLerpRatio;
        }
    }

    return 0.f;
}

void CEffectObject::Update_Gravity_Force(float fLifeRatio)
{
    // << 자연 중력 계산 >>
    float fGlobalCurveMod = 1.f;
    if (m_tEffectDesc._bUseGlobalGravityCurve)
        fGlobalCurveMod = Sample_GravityCurve(m_tEffectDesc._vecGlobalGravityCurve, fLifeRatio);

    // 최종 자연 중력 벡터 = 방향 * 기본 세기 * 전체 배수 * 커브 배수
    Vec3 vFianlGlobalGravity = m_tEffectDesc._Effect_GravityDir * m_tEffectDesc._Effect_Gravity_Value * m_tEffectDesc._Effect_GravityModifier * fGlobalCurveMod;

    // << 외부 중력 계산 >>
    float fExternalCurveMod = 1.0f;
    if (m_tEffectDesc._bUseExternalForceCurve)
        fExternalCurveMod = Sample_GravityCurve(m_tEffectDesc._vecExternalForceCurve, fLifeRatio);

    // Force Field 
    float fFinalExternalStrength = m_tEffectDesc.fExternalForceStrength * fExternalCurveMod;

    // TODO: 이 값들을 Compute Shader에게 넘겨주기.
    //       vecCurve의 fTime은 반드시 오름차순으로 정렬해주기. Sort 한번 해주는 것이 좋다.
    //       fDeltaTime이 아주 작을 떄 나누기 에러가 날 수 있다. 조심하자.

}

float CEffectObject::Sample_RotationCurve(const vector<Rotation_CurveKey>& vecCurve, float fLifeRatio)
{
    if (vecCurve.empty()) return 0.f;
    if (vecCurve.size() == 1) return vecCurve[0].fValue;

    if (fLifeRatio <= vecCurve.front().fTimeKey) return vecCurve.front().fValue;
    if (fLifeRatio >= vecCurve.back().fTimeKey) return vecCurve.back().fValue;

    for (size_t i = 0; i < vecCurve.size() - 1; ++i)
    {
        if (fLifeRatio >= vecCurve[i].fTimeKey && fLifeRatio <= vecCurve[i + 1].fTimeKey)
        {
            float fDeltaTime = vecCurve[i + 1].fTimeKey - vecCurve[i].fTimeKey;
            float fLerpRatio = (fLifeRatio - vecCurve[i].fTimeKey) / fDeltaTime;

            // 각 축별로 부드럽게 보간해서 현재 시점의 회전값 반환
            return vecCurve[i].fValue + (vecCurve[i + 1].fValue - vecCurve[i].fValue) * fLerpRatio;
        }
    }

    return 0.f;
}

void CEffectObject::Update_Rotation_Lerp(float fDT, float fRatio)
{
    if (m_tEffectDesc._bUseRotationCurve)
    {
        // 1. 각 축별 커브에서 '진행도 비율' 가져오기.
        // 이때 그래프의 Y축은 0.0(시작) ~ 1.0(완료) 비율이어야 해!
        Vec3 vCurveRatio;
        vCurveRatio.x = Sample_RotationCurve(m_tEffectDesc._vecRotationCurveX, fRatio);
        vCurveRatio.y = Sample_RotationCurve(m_tEffectDesc._vecRotationCurveY, fRatio);
        vCurveRatio.z = Sample_RotationCurve(m_tEffectDesc._vecRotationCurveZ, fRatio);

        // [핵심 공식] 
        // 최종 회전 = 시작 각도 + (전체 목표 회전량 * 현재 커브 진행 비율)
        // 이렇게 하면 커브 값이 1.0에 도달했을 때 딱 TargetRotation만큼만 돌아있게 돼.
        Vec3 vFinalRot;
        vFinalRot.x = m_tEffectDesc._Effect_StartRotation.x + (m_tEffectDesc._Effect_TargetRotation.x * vCurveRatio.x);
        vFinalRot.y = m_tEffectDesc._Effect_StartRotation.y + (m_tEffectDesc._Effect_TargetRotation.y * vCurveRatio.y);
        vFinalRot.z = m_tEffectDesc._Effect_StartRotation.z + (m_tEffectDesc._Effect_TargetRotation.z * vCurveRatio.z);

        // 3. 변환 후 적용
        Get_Component<CTransform>()->Rotation(
            DirectX::XMConvertToRadians(vFinalRot.x),
            DirectX::XMConvertToRadians(vFinalRot.y),
            DirectX::XMConvertToRadians(vFinalRot.z)
        );
    }
}

void CEffectObject::Update_UV_Scroll_Curve(float fRatio)
{
    if (m_tEffectDesc._bUseUVScrollCurve)
    {
        // Rotataion 커브 재활용이요
        float fCurveX = Sample_RotationCurve(m_tEffectDesc._vecUVScrollCurveX, fRatio);

        float fCurveY = Sample_RotationCurve(m_tEffectDesc._vecUVScrollCurveY, fRatio);
        // 결과값을 저장한다.
        m_vScrollOffset.x = fCurveX;
        m_vScrollOffset.y = fCurveY;
    }
}

CEffectObject* CEffectObject::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEffectObject* pInstance = new CEffectObject(eType, pDevice, pDeviceContext);

    if (pInstance == nullptr)
    {
        MSG_BOX("Create to Fail : EffectObject");
        Safe_Release(pInstance);
        return nullptr;
    }

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Initialize to Fail : EffectObject");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CEffectObject::Clone(void* pArg)
{
    CEffectObject* pClone = new CEffectObject(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CEffectObject::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CEffectObject::Free()
{
    Super::Free();
}
