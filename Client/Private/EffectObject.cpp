#include "pch.h"
#include "EffectObject.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Model.h"
#include "Texture.h"
#include "mesh.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Mesh.h"
#include "DataStruct_Effect.h"
#include "StructuredBuffer.h"
#include "GameInstance.h"

#define PLAY 0
#define PAUSE 1
#define RESET 2
#define STOP 3

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :Super(pDevice, pDeviceContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
    :Super(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::Initialize(pArg)");
        return E_FAIL;
    }

    if (FAILED(EffectDesc_Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::EffectDesc_Initialize(pArg)");
        return E_FAIL;
    }

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectObject::EffectDesc_Initialize(void* pArg)
{
    Effect_Desc* pEffectDesc = static_cast<Effect_Desc*>(pArg);

    if (pEffectDesc == nullptr)
        return E_FAIL;

    m_tEffectDesc = *pEffectDesc;
    m_tOriginEffectDesc = *pEffectDesc;
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

    m_pModel = Get_Component<CModel>();
    m_pTexture = Get_Component<CTexture>();
    m_pShader = Get_Component<CShader>();
    m_pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
    m_pTransform = Get_Component<CTransform>();


    if (pSRV = m_pComputeShader->Get_SRV("g_GravityCurve"))
    {
        if (m_tEffectDesc.Data._vecGlobalGravityCurve.size() != 0)
            pSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(DTO::Gravity_CurveKey), m_tEffectDesc.Data._vecGlobalGravityCurve.size());

        else
            pSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(DTO::Gravity_CurveKey), 1);

        pSRV->SetResource(pSB->Get_SRV());
    }
    m_pComputeShader->Bind_InputStructuredBuffer(1, pSRV, pSB);

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Shader()
{
    // ========   Rendering Shader   ========
    {
        CShader::SHADER_ORIGIN_DESC ShaderDesc = {};
        wstring ShaderTag = L"Prototype_Component_";
        Add_Component<CShader>(0, ShaderTag + m_tEffectDesc.Data._Effect_Shader_Tag, &ShaderDesc);
        Get_Component<CShader>()->Set_Pass(m_tEffectDesc.Data._Effect_ShaderPass);
    }

    // ========   Compute Shader   ========
    {
        CComputeShader::ComShaderCopyDesc ShaderDesc = {};
        ShaderDesc.Output_SRVBuffer_Name = "INSTANCE_RESULT_SRV";
        // 입력 버퍼
        ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
        ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
        ShaderDesc.Input_StructBuffer.iNumElements = m_tEffectDesc.Data._Effect_MaxParticle;
        ShaderDesc.InputBufferNum = 2;

        // 출력 버퍼
        ShaderDesc.OutPut_StructBuffer.sBufferName = "INSTANCE_OUTPUT";
        ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(EFFECT_INSTANCE);
        ShaderDesc.OutPut_StructBuffer.iNumElements = m_tEffectDesc.Data._Effect_MaxParticle;

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
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DIFFUSE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_NoiseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::NOISE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_MaskingTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::MASKING));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_GradationTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GRADATION));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_DissolveTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DISSOLVE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_GlowTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GLOW));
        }
    }

    else
    {
        Add_Component<CTexture>(0, L"Prototype_Component_Texture_Empty", &desc);

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DIFFUSE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_NoiseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::NOISE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_MaskingTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::MASKING));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_GradationTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GRADATION));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_DissolveTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DISSOLVE));
            pInstance->Add_DefaultTexture(s + m_tEffectDesc.Data._Effect_GlowTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GLOW));
        }
    }

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Model(void* pArg)
{
    Effect_Desc* pDesc = static_cast<Effect_Desc*>(pArg);

    CModel::MODEL_COPY_DESC pModelDesc = {};
    wstring s = L"Prototype_Component_Model_";

    if (m_tEffectDesc.Data.eEffectParticleType == (_uint)DTO::E_PARTICLETYPE::MESH)
    {
        if (FAILED(Add_Component<CModel>(0, s + m_tEffectDesc.Data._Effect_Model_Tag, &pModelDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffectObject::Ready_Component_Buffer(void* pArg)
{
    Effect_Desc* pDesc = static_cast<Effect_Desc*>(pArg);

    switch (m_tEffectDesc.Data.eEffectParticleType)
    {
        case (_uint)DTO::E_PARTICLETYPE::PARTICLE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
            pParticleDesc.iInstnaceCount = m_tEffectDesc.Data._Effect_MaxParticle;
            pParticleDesc.isLoop = m_tEffectDesc.Data._Effect_Looping;
            pParticleDesc.vLifeTime.x = 0.f;
            pParticleDesc.vLifeTime.y = m_tEffectDesc.Data._Effect_LifeTime;
            pParticleDesc.vRange = m_tEffectDesc.Data._Effect_Range;
            pParticleDesc.m_fStartSpeeds = m_tEffectDesc.Data._Effect_StartSpeed;
            pParticleDesc.vSize = m_tEffectDesc.Data._Effect_ParticleSize;
            pParticleDesc.vSpeed = Vec2{ 1.f, 2.f };
            pParticleDesc.iRandomFlags = m_tEffectDesc.Data.iRandomFlags;
            pParticleDesc.pOwner = this;
            pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
            pParticleDesc.EmissionFlagType = m_tEffectDesc.Data._Effect_EmissionType;

            if (FAILED(Add_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)))))
                return E_FAIL;
            break;
        }
        case (_uint)DTO::E_PARTICLETYPE::MESH:
        {
            // 모델이 있을 때.
            if (Get_Component<CModel>())
            {
                CModel* pInstance = Get_Component<CModel>();
                CVIBuffer_Particle_Mesh::PARTICLE_Mesh_ORIGIN_DESC MeshBufferDesc = {};
                MeshBufferDesc.iInstnaceCount = m_tEffectDesc.Data._Effect_MaxParticle;
                MeshBufferDesc.isLoop = m_tEffectDesc.Data._Effect_Looping;
                MeshBufferDesc.vLifeTime.x = 0.f;
                MeshBufferDesc.vLifeTime.y = m_tEffectDesc.Data._Effect_LifeTime;
                MeshBufferDesc.vRange = m_tEffectDesc.Data._Effect_Range;
                MeshBufferDesc.m_fStartSpeeds = m_tEffectDesc.Data._Effect_StartSpeed;
                MeshBufferDesc.vSize = m_tEffectDesc.Data._Effect_ParticleSize;
                MeshBufferDesc.vSpeed = Vec2{ 1.f, 2.f };
                MeshBufferDesc.iRandomFlags = m_tEffectDesc.Data.iRandomFlags;
                MeshBufferDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
                MeshBufferDesc.pModel = pInstance;
                MeshBufferDesc.pOwner = this;
                MeshBufferDesc.EmissionFlagType = m_tEffectDesc.Data._Effect_EmissionType;
                if (FAILED(Add_Component<CVIBuffer_Particle_Mesh>(static_cast<CVIBuffer_Particle_Mesh*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Mesh", &MeshBufferDesc)))))
                    return E_FAIL;
            }
            break;
        }
        case (_uint)DTO::E_PARTICLETYPE::TEXTURE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC pParticleDesc = {};
            pParticleDesc.iInstnaceCount = m_tEffectDesc.Data._Effect_MaxParticle;
            pParticleDesc.isLoop = m_tEffectDesc.Data._Effect_Looping;
            pParticleDesc.vLifeTime.x = 0.f;
            pParticleDesc.vLifeTime.y = m_tEffectDesc.Data._Effect_LifeTime;
            pParticleDesc.vRange = m_tEffectDesc.Data._Effect_Range;
            pParticleDesc.m_fStartSpeeds = m_tEffectDesc.Data._Effect_StartSpeed;
            pParticleDesc.vSize = m_tEffectDesc.Data._Effect_ParticleSize;
            pParticleDesc.vSpeed = Vec2{ 1.f, 2.f };
            pParticleDesc.iRandomFlags = m_tEffectDesc.Data.iRandomFlags;
            pParticleDesc.pOwner = this;
            pParticleDesc.pComputeShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));

            if (FAILED(Add_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Point", &pParticleDesc)))))
                return E_FAIL;
        }
    }

    return S_OK;
}


HRESULT CEffectObject::Bind_ShaderResource()
{
    if (m_pShader == nullptr) return S_OK;
    m_pShader->Set_Pass(m_tEffectDesc.Data._Effect_ShaderPass);
    m_pShader->Bind_TransformData(m_matCombinedWorld);

    if (m_tEffectDesc.Data._Effect_ShaderPass == 3)
    {
        if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::SceneHDR_Copy, m_pShader)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(ERenderTarget::Depth, m_pShader)))
            return E_FAIL;
    }


    // 셰이더에 던질 구조체 작성하기.
    {
        SHADER_EFFECT_DESC pDesc = {};
        // TextureFlag
        pDesc.iTextureFlags = m_tEffectDesc.Data._Effect_TextureFlag;
        // RenderFlag
        pDesc.iRenderFlags = m_tEffectDesc.Data._Effect_RenderFlag;
        // SamplerStateFlag
        pDesc.iSamplerStateFlags = m_tEffectDesc.Data._Effect_SamplerStateFlag;
        // DiscardValue
        pDesc.iDiscardValue = m_tEffectDesc.Data._Effect_DiscardValue;
        // Texture_RotationFlag
        pDesc.iRotationFlags = m_tEffectDesc.Data._Effect_TextureRotationFlag;
        // Texture_OperatorFlag
        pDesc.iOperatorFlags = m_tEffectDesc.Data._Effect_TextureOperatorFlag;
        pDesc.vUVOffset = m_tEffectDesc.Data._Effect_UV_Offset;

        pDesc.SpriteColCount = m_tEffectDesc.Data._Effect_TileCount.x;
        pDesc.SpriteRowCount = m_tEffectDesc.Data._Effect_TileCount.y;
        pDesc.CurSpriteIndex = m_tEffectDesc.Data.m_iCurSpriteNumber;
        pDesc.fLifeRatio = m_tEffectDesc.Data._Effect_ApearRatio;

        pDesc.vDistortionScale = m_tEffectDesc.Data._Effect_DistortionScale;
        pDesc.vEffectColor = m_tEffectDesc.Data._Effect_Color;
        pDesc.vScrollOffset = m_vScrollOffset;

        pDesc.DiffuseTexture_ScrollWeight = m_tEffectDesc.Data._Effect_DiffuseTexture_ScrollWeight;
        pDesc.NoiseTexture_ScrollWeight = m_tEffectDesc.Data._Effect_NoiseTexture_ScrollWeight;
        pDesc.MaskingTexture_ScrollWeight = m_tEffectDesc.Data._Effect_MaskingTexture_ScrollWeight;
        pDesc.GradationTexture_ScrollWeight = m_tEffectDesc.Data._Effect_GradationTexture_ScrollWeight;

        m_pShader->Bind_EffectData(pDesc);

        // Compute 셰이더가 들고있는 SRV를, Default Shader한테 SRV 꽂아주기.
        {
            ID3D11ShaderResourceView* pResultSRV = m_pComputeShader->Get_Output_Buffer()->Get_SRV();
            ID3DX11EffectShaderResourceVariable* pSRVar = m_pShader->Get_SRV("INSTANCE_OUTPUT");
            if (pSRVar)
                pSRVar->SetResource(pResultSRV);
        }
    }

    // 텍스처 바인딩
    {
        if (m_pTexture)
            m_pTexture->Bind_ShaderResourceBuffer(m_pShader);
    }

    if (m_tEffectDesc.Data.eEffectParticleType != (_uint)DTO::E_PARTICLETYPE::MESH || !m_pModel)
    {
        CVIBuffer_Particle_Point* pInstance = static_cast<CVIBuffer_Particle_Point*>(Get_Component<CVIBuffer_Particle_Point>());

        if (pInstance)
        {
            pInstance->Bind_Resource();
            m_pShader->Apply();
            pInstance->Render();
        }
    }

    else if (m_tEffectDesc.Data.eEffectParticleType == (_uint)DTO::E_PARTICLETYPE::MESH && m_pModel)
    {
        CVIBuffer_Particle_Mesh* pInstance = static_cast<CVIBuffer_Particle_Mesh*>(Get_Component<CVIBuffer_Particle_Mesh>());

        if (pInstance)
        {
            pInstance->Bind_Resource();
            m_pShader->Apply();
            pInstance->Render();
        }
    }

    return S_OK;
}

HRESULT CEffectObject::Bind_Curve_To_GPU()
{
    auto& vecCurve = m_tEffectDesc.Data._vecGlobalGravityCurve; // CPU에 있는 커브 데이터
    if (vecCurve.empty()) return E_FAIL;

    m_pComputeShader->Bind_InputStructuredBuffer_Data(1, vecCurve.data(), sizeof(DTO::Gravity_CurveKey), (_uint)vecCurve.size());

    EFFECT_CURVEINFO desc;
    desc.g_iGravityKeyCount = vecCurve.size();
    m_pComputeShader->Bind_Compute_EffectCurveData(desc);

    return S_OK;
}


HRESULT CEffectObject::Awake(const _uint iCurrentLevelID)
{
    // 절대로 절대로 Loader에서 불리면 안된다.
    return S_OK;
}

void CEffectObject::Update_Priority(const _float fDT)
{
    Super::Update_Priority(fDT);
}

void CEffectObject::Update(const _float fTimeDelta)
{
    // 시간 누적 (Timeflag가 PLAY일 때만,)
    float TimeFlag = (m_tEffectDesc.Data._Effect_TimeFlag == PLAY) ? 1.f : 0.f;
    _float TimeT = m_tEffectDesc.Data._Effect_PlayBackSpeed * fTimeDelta * TimeFlag;
    m_fTimeAccumulation += TimeT;

    // Start Delay 체크
    if (m_fTimeAccumulation < m_tEffectDesc.Data._Effect_StartDelay)
    {
        m_bIsStarted = false;
        return;
    }
    m_bIsStarted = true;

    _float fActiveTime = m_fTimeAccumulation - m_tEffectDesc.Data._Effect_StartDelay;

    // Duration 및 Loop 제어 설정
    if (fActiveTime >= m_tEffectDesc.Data._Effect_Duration)
    {
        if (m_tEffectDesc.Data._Effect_Looping)
        {
            m_fTimeAccumulation = m_tEffectDesc.Data._Effect_StartDelay + fmod(fActiveTime, m_tEffectDesc.Data._Effect_Duration);
            fActiveTime = m_fTimeAccumulation - m_tEffectDesc.Data._Effect_StartDelay;
        }

        else
        {
            if (fActiveTime >= m_tEffectDesc.Data._Effect_Duration + m_tEffectDesc.Data._Effect_LifeTime)
            {
                m_bIsEffectFinish = true;
            }
        }
    }
    _float fRatio = fActiveTime / m_tEffectDesc.Data._Effect_Duration;
    if (fRatio > 1.f) fRatio = 1.f;

    Vec3 vCurrentScale = Vec3::Lerp(m_tEffectDesc.Data._Effect_StartScale, m_tEffectDesc.Data._Effect_EndScale, fRatio);
    Get_Component<CTransform>()->Set_Scale(vCurrentScale);

    // GPU에 백터 바인딩.
    Bind_Curve_To_GPU();
    TimeCalculate(TimeT);
    Update_UV_Scroll_Curve(fRatio);
    Update_Rotation_Lerp(TimeT, fRatio);
    Update_Gravity_Force();   // 중력값 전달.
    Super::Update(TimeT);

    auto CTShader = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader"));
    if (CTShader)
    {
        // Object의 TimeFlag가 PLAY라면 그대로 전달하여 CS가 멈추지 않게 함
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(CTShader, Vec3{}, m_vFinalGravity, TimeT, m_tEffectDesc.Data._Effect_TimeFlag, (DTO::E_SHAPETYPE)m_tEffectDesc.Data._Effect_ShapeType);
    }
}

void CEffectObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CEffectObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

    if (m_tEffectDesc.Data._Effect_ShaderPass != 3)
        m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONELIGHT, this);

    else
        m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::DISTOTION, this);

    Super::Update_CombinedWorldMatrix(m_pMatParent);
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
    return _bool();
}

_bool CEffectObject::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    return false;
}

void CEffectObject::Set_Dead(const wstring& wstrLayerTag)
{
}

HRESULT CEffectObject::Spawn_FromPool(void* pArg)
{
    m_tEffectDesc = m_tOriginEffectDesc;
    TimeFlagRequest(RESET);

    return S_OK;
}
HRESULT CEffectObject::Despawn_FromPool()
{
    TimeFlagRequest(RESET);

    return S_OK;
}

void CEffectObject::LoopState_Change(E_LoopState eState)
{
    // TODO : 

    switch (eState)
    {
    case E_LoopState::LOOP_START:
    {
        break;
    }
    case E_LoopState::LOOP_END:
    {
        break;
    }
    }
}

void CEffectObject::TimeFlagRequest(_uint iTimeFlag)
{
    m_tEffectDesc.Data._Effect_TimeFlag = iTimeFlag;

    if (iTimeFlag == RESET)
    {
        m_bIsEffectFinish = false;
        m_bIsStarted = false;
        m_fTimeAccumulation = 0.f;
        m_vScrollOffset = Vec2{ 0.f, 0.f };
        m_vAccumulatedRotation = { 0.f, 0.f, 0.f };
        m_vScrollOffset = { 0.f, 0.f }; // 스크롤 값도 완전 초기화

        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance && m_pComputeShader)
            pInstance->Update_Simulation(m_pComputeShader, Vec3{}, m_vFinalGravity, 0.f, RESET, (DTO::E_SHAPETYPE)m_tEffectDesc.Data._Effect_ShapeType);

        // 리셋 직후 바로 PLAY 상태로 전이시켜서 셰이더가 다음 루프를 돌게 함
        m_tEffectDesc.Data._Effect_TimeFlag = PLAY;
    }

    else if (iTimeFlag == STOP)
    {
        m_bIsEffectFinish = false;
        m_bIsStarted = false;
        m_fTimeAccumulation = 0.f;
        m_vScrollOffset = Vec2{ 0.f, 0.f };
        m_vAccumulatedRotation = { 0.f, 0.f, 0.f };

        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance && m_pComputeShader)
            pInstance->Update_Simulation(m_pComputeShader, Vec3{}, m_vFinalGravity, 0.f, STOP, (DTO::E_SHAPETYPE)m_tEffectDesc.Data._Effect_ShapeType);

        m_fTimeAccumulation = 0.f;
        // Stop은 PLAY로 바꾸지 않고 그대로 둠
    }
}

void CEffectObject::TimeCalculate(const _float fDT)
{
    // Start Delay를 제외한 순수 실행 시간 기반 진행률
    float fActiveTime = m_fTimeAccumulation - m_tEffectDesc.Data._Effect_StartDelay;
    if (fActiveTime < 0.f) fActiveTime = 0.f;

    // ======= [스크롤 값] ==========
    m_vScrollOffset += fDT * m_tEffectDesc.Data._Effect_ScrollSpeed;

    // ======= [스프라이트 애니메이션] =======
    if ((m_tEffectDesc.Data._Effect_RenderFlag & (1 << 5)) && m_tEffectDesc.Data._Effect_bPlayAnim)
    {
        _uint iTotalFrame = m_tEffectDesc.Data._Effect_TileCount.x * m_tEffectDesc.Data._Effect_TileCount.y;
        if (iTotalFrame > 0)
        {
            _uint iFrame = (_uint)(fActiveTime * m_tEffectDesc.Data._Effect_AnimSpeed);

            if (m_tEffectDesc.Data._Effect_Looping)
                m_tEffectDesc.Data.m_iCurSpriteNumber = iFrame % iTotalFrame;
            else
                m_tEffectDesc.Data.m_iCurSpriteNumber = std::min(iFrame, iTotalFrame - 1);
        }
    }
}

void CEffectObject::Update_Gravity_Force()
{
    m_vFinalGravity = m_tEffectDesc.Data._Effect_GravityDir * m_tEffectDesc.Data._Effect_Gravity_Value * m_tEffectDesc.Data._Effect_GravityModifier;
}

float CEffectObject::Sample_RotationCurve(const vector<DTO::Rotation_CurveKey>& vecCurve, float fLifeRatio)
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
    if (m_tEffectDesc.Data._bUseRotationCurve)
    {
        // 1. 각 축별 커브에서 '진행도 비율' 가져오기.
        // 이때 그래프의 Y축은 0.0(시작) ~ 1.0(완료) 비율이어야 해!
        Vec3 vCurveRatio;
        vCurveRatio.x = Sample_RotationCurve(m_tEffectDesc.Data._vecRotationCurveX, fRatio);
        vCurveRatio.y = Sample_RotationCurve(m_tEffectDesc.Data._vecRotationCurveY, fRatio);
        vCurveRatio.z = Sample_RotationCurve(m_tEffectDesc.Data._vecRotationCurveZ, fRatio);

        // [핵심 공식] 
        // 최종 회전 = 시작 각도 + (전체 목표 회전량 * 현재 커브 진행 비율)
        // 이렇게 하면 커브 값이 1.0에 도달했을 때 딱 TargetRotation만큼만 돌아있게 돼.
        Vec3 vFinalRot;
        vFinalRot.x = m_tEffectDesc.Data._Effect_StartRotation.x + (m_tEffectDesc.Data._Effect_TargetRotation.x * vCurveRatio.x);
        vFinalRot.y = m_tEffectDesc.Data._Effect_StartRotation.y + (m_tEffectDesc.Data._Effect_TargetRotation.y * vCurveRatio.y);
        vFinalRot.z = m_tEffectDesc.Data._Effect_StartRotation.z + (m_tEffectDesc.Data._Effect_TargetRotation.z * vCurveRatio.z);

        // 3. 변환 후 적용
        m_pTransform->Rotation(
            DirectX::XMConvertToRadians(vFinalRot.x),
            DirectX::XMConvertToRadians(vFinalRot.y),
            DirectX::XMConvertToRadians(vFinalRot.z)
        );
    }
}

void CEffectObject::Update_UV_Scroll_Curve(float fRatio)
{
    if (m_tEffectDesc.Data._bUseUVScrollCurve)
    {
        // Rotataion 커브 재활용이요
        float fCurveX = Sample_RotationCurve(m_tEffectDesc.Data._vecUVScrollCurveX, fRatio);

        float fCurveY = Sample_RotationCurve(m_tEffectDesc.Data._vecUVScrollCurveY, fRatio);
        // 결과값을 저장한다.
        m_vScrollOffset.x = fCurveX;
        m_vScrollOffset.y = fCurveY;
    }
}

CEffectObject* CEffectObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEffectObject* pInstance = new CEffectObject(pDevice, pDeviceContext);

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

    Safe_Release(pSRV);
    Safe_Release(pSB);
}
