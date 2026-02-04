#include "pch.h"
#include "EffectObject.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "mesh.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Mesh.h"
#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CPartObject(pDevice, pDeviceContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
    :CPartObject(rhs)
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

    if (FAILED(Component_Setting(pArg)))
        return E_FAIL;

    if (FAILED(EffectDesc_Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::EffectDesc_Initialize(pArg)");
        return E_FAIL;
    }

    Effect_Desc* pDesc = static_cast<Effect_Desc*>(pArg);
    if (pDesc != nullptr)
        m_tEffectDesc = *pDesc;

    return S_OK;
}

HRESULT CEffectObject::Component_Setting(void* pArg)
{
    _uint StaticLevel = 0;

    if (FAILED(Add_Component<CVIBuffer_Particle_Point>(StaticLevel, L"Prototype_Component_VIBuffer_Particle_Point", nullptr)))
        return E_FAIL;

    // Default 값으로 Shader 세팅
    {
        CShader::SHADER_ORIGIN_DESC ShaderDesc = {};
        if (FAILED(Add_Component<CShader>(StaticLevel, L"Prototype_Component_Shader_VtxEffectParticle", &ShaderDesc)))
            return E_FAIL;
    }

    // Default 값으로 Texture 세팅
    {
        CTexture::TEXTURE_COMPONENT_ORIGIN_DESC desc = {};
        if (FAILED(Add_Component<CTexture>(StaticLevel, L"Prototype_Component_Texture_Default", &desc)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CEffectObject::EffectDesc_Initialize(void* pArg)
{
    Effect_Desc* pEffectDesc = static_cast<Effect_Desc*>(pArg);

    if (pEffectDesc == nullptr)
        return E_FAIL;

    m_tEffectDesc = *pEffectDesc;
    return S_OK;
}

void CEffectObject::Set_EffectDesc(const Effect_Desc& Desc)
{
    m_tEffectDesc = Desc;

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
        case DTO::E_PARTICLETYPE::PARTICLE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC PointBufferDesc = {};
            PointBufferDesc.pOwner = this;
            Change_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Point", &PointBufferDesc)));
            break;
        }
        case DTO::E_PARTICLETYPE::MESH:
        {
            if (Get_Component<CModel>())
            {
                CModel* pInstance = Get_Component<CModel>();
                CVIBuffer_Particle_Mesh::PARTICLE_Mesh_ORIGIN_DESC MeshBufferDesc = {};
                MeshBufferDesc._Model = pInstance;

                Change_Component<CVIBuffer_Particle_Mesh>(static_cast<CVIBuffer_Particle_Mesh*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Mesh", &MeshBufferDesc)));
            }
            break;
        }
        case DTO::E_PARTICLETYPE::TEXTURE:
        {
            CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC PointBufferDesc = {};
            PointBufferDesc.pOwner = this;
            Change_Component<CVIBuffer_Particle_Point>(static_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0, L"Prototype_Component_VIBuffer_Particle_Point", &PointBufferDesc)));
            break;
        }
        }
    }
}

void CEffectObject::Particle_Setting()
{
    if (m_tEffectDesc.eEffectType == DTO::E_EFFECTTYPE::Particle)
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        auto pDesc = pInstance->Get_ParticleDesc();

        CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC desc = {};

        desc.iInstnaceCount = m_tEffectDesc._Effect_MaxParticle;
        desc.isLoop = m_tEffectDesc._Effect_Looping;
        desc.vLifeTime.x = pDesc.vLifeTime.x;
        desc.vLifeTime.y = m_tEffectDesc._Effect_LifeTime;
        desc.vRange = m_tEffectDesc._Effect_Range;
        desc.m_fStartSpeeds = m_tEffectDesc._Effect_StartSpeed;
        desc.vSize = m_tEffectDesc._Effect_ParticleSize;
        desc.vSpeed = Vec2{ 0.f, 3.f };
        desc.isRandomSeed = m_tEffectDesc._Effect_IsRandomSeed;
        desc.pModel = Get_Component<CModel>();

        pInstance->Set_ParticleDesc(desc);
    }
}

void CEffectObject::Model_Setting(const wstring& ModelName)
{
    _uint staticLevel = 0;

    if (m_tPrevEffectDesc._Effect_Model_Tag != m_tEffectDesc._Effect_Model_Tag)
    {
        CModel::MODEL_COPY_DESC pDesc = {};
        wstring s = L"Prototype_Component_Model_";

        if (Get_Component<CModel>())
            Change_Component<CModel>(static_cast<CModel*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, staticLevel, s + ModelName, &pDesc)));

        else
            if (FAILED(Add_Component<CModel>(staticLevel, s + ModelName, &pDesc)))
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
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GRADATION));
        }
    }

    else
    {
        Add_Component<CTexture>(0, s + m_tEffectDesc._Effect_DiffuseTexture_Tag, &desc);

        CTexture* pInstance = Get_Component<CTexture>();
        if (pInstance)
        {
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_DiffuseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::DIFFUSE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_NoiseTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::NOISE));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_MaskingTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::MASKING));
            Get_Component<CTexture>()->Add_DefaultTexture(s + m_tEffectDesc._Effect_GradationTexture_Tag, ENUM_TO_UINT(DTO::E_TEXTURETYPE::GRADATION));
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
    pShader->Bind_TransformData(m_matCombinedWorld);

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
        pDesc.vPadding1 = SimpleMath::Vector2(0.f, 0.f);

        pDesc.SpriteColCount = m_tEffectDesc._Effect_TileCount.x;
        pDesc.SpriteRowCount = m_tEffectDesc._Effect_TileCount.y;
        pDesc.CurSpriteIndex = m_tEffectDesc.m_iCurSpriteNumber;
        pDesc.Padding2 = { 0.f };

        pDesc.vDistortionScale = m_tEffectDesc._Effect_DistortionScale;
        pDesc.vEffectColor = m_tEffectDesc._Effect_Color;
        pDesc.vScrollOffset = m_vScrollOffset;
        pShader->Bind_EffectData(pDesc);
    }

    // 텍스처 바인딩
    {
        CTexture* pTexture = Get_Component<CTexture>();
        if (pTexture)
            pTexture->Bind_ShaderResourceBuffer(Get_Component<CShader>());
    }

    if (m_tEffectDesc.eEffectParticleType != DTO::E_PARTICLETYPE::MESH)
    {
        CVIBuffer_Particle_Point* pInstance = static_cast<CVIBuffer_Particle_Point*>(Get_Component<CVIBuffer_Particle_Point>());

        if (pInstance)
        {
            pInstance->Bind_Resource();
            pShader->Apply();
            pInstance->Render();
        }
    }

    else if (m_tEffectDesc.eEffectParticleType == DTO::E_PARTICLETYPE::MESH)
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
    // 절대로 절대로 Loader에서 불리면 안된다.
    return S_OK;
}

void CEffectObject::Update_Priority(const _float fDT)
{
    Super::Update_Priority(fDT);
}

void CEffectObject::Update(const _float fTimeDelta)
{
    _float TimeT = m_tEffectDesc._Effect_PlayBackSpeed * fTimeDelta;
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
            m_fTimeAccumulation = m_tEffectDesc._Effect_StartDelay; // 리셋
            fActiveTime = 0.f;
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
    Super::Update(TimeT);
    // == 스크롤 값 == 

    switch (m_tEffectDesc._Effect_ShapeType)
    {
    case DTO::E_SHAPETYPE::NONE:
        break;

    case DTO::E_SHAPETYPE::SPREAD:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(Get_Component<CShader>(),Vec3{}, TimeT, E_PARTICLE_MOVESTATE::SPREAD);
        break;
    }
    case DTO::E_SHAPETYPE::DROP:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(Get_Component<CShader>(), Vec3{}, TimeT, E_PARTICLE_MOVESTATE::DROP);
        break;
    }
    case DTO::E_SHAPETYPE::RISE:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(Get_Component<CShader>(), Vec3{}, TimeT, E_PARTICLE_MOVESTATE::RISE);
        break;
    }
    case DTO::E_SHAPETYPE::MESH:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(Get_Component<CShader>(), Vec3{}, TimeT, E_PARTICLE_MOVESTATE::RISE);
        break;
    }

    case DTO::E_SHAPETYPE::STRAIGHT:
    {
        CVIBuffer_Particle_Point* pInstance = Get_Component<CVIBuffer_Particle_Point>();
        if (pInstance) pInstance->Update_Simulation(Get_Component<CShader>(), Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK), TimeT, E_PARTICLE_MOVESTATE::STRAIGHT);
        break;
    }
    }
}

void CEffectObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CEffectObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

    m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONELIGHT, this);
    Super::Update_CombinedWorldMatrix(m_pMatParent);
}

HRESULT CEffectObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    switch (m_tEffectDesc.eEffectType)
    {
    case DTO::E_EFFECTTYPE::Particle:
        // 파티클 타입에 따라 어떤 형식으로 분사하고.
        // 어떤 종류를 파티클로 지정할 것인지. (Mesh, Particle, Texture) 3가지 타입.
        // Texture일 때, 빌보드를 먹일 것인가?
        Bind_ShaderResource_Particles();
        break;

    case DTO::E_EFFECTTYPE::Mesh:
        // 매쉬를 그리고 스크롤 때리는 것. 로직 작성
        Bind_ShaderResource_Meshes();
        break;

    case DTO::E_EFFECTTYPE::Trail:
        // 무조건 SIMULATIONSPACE_ WORLD 타입으로 
        Bind_ShaderResource_Trails();
        break;
    }

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

void CEffectObject::Bind_ShaderResource_Particles()
{
    switch (m_tEffectDesc.eEffectParticleType)
    {
    case DTO::E_PARTICLETYPE::NONE:
        break;

    case DTO::E_PARTICLETYPE::PARTICLE:
        Get_Component<CShader>()->Set_Pass(0);
        break;

    case DTO::E_PARTICLETYPE::TEXTURE:
        Get_Component<CShader>()->Set_Pass(0);
        break;

    case DTO::E_PARTICLETYPE::MESH:
        Get_Component<CShader>()->Set_Pass(0);
        break;
    }
    // 어떤 종류를 파티클로 지정할 것인지. (Mesh, Particle, Texture) 3가지 타입.

    // Texture일 때, 빌보드를 먹일 것인가?
}

void CEffectObject::TimeReset(Effect_Desc Desc)
{
    m_vScrollOffset = Vec2{ 0.f, 0.f };
    m_fTimeAccumulation = 0.f; // 시간 초기화
    m_bIsStarted = false;      // 시작 상태 초기화

    // Transform 스케일도 Start Scale로 원복
    Get_Component<CTransform>()->Set_Scale(m_tEffectDesc._Effect_StartScale);

    m_tPrevEffectDesc = m_tEffectDesc = Desc;
    Set_EffectDesc(m_tEffectDesc);
}

void CEffectObject::TimeCalculate(const _float fDT)
{
    // Start Delay를 제외한 순수 실행 시간 기반 진행률
    _float fActiveTime = m_fTimeAccumulation - m_tEffectDesc._Effect_StartDelay;
    if (fActiveTime < 0.f) fActiveTime = 0.f;

    // ======= [스크롤 값] ==========
    m_vScrollOffset.x += fDT * m_tEffectDesc._Effect_ScrollSpeed.x;
    m_vScrollOffset.y += fDT * m_tEffectDesc._Effect_ScrollSpeed.y;

    // ======= [스프라이트 애니메이션] =======
    if ((m_tEffectDesc._Effect_RenderFlag & (1 << 5)) && m_tEffectDesc._Effect_bPlayAnim)
    {
        _uint iTotalFrame = m_tEffectDesc._Effect_TileCount.x * m_tEffectDesc._Effect_TileCount.y;
        if (iTotalFrame > 0)
        {
            // TimeAccumulation 대신 fActiveTime을 사용하여 Delay 이후부터 0프레임 시작
            m_tEffectDesc.m_iCurSpriteNumber = (_uint)(fActiveTime * m_tEffectDesc._Effect_AnimSpeed) % iTotalFrame;
        }
    }
}

void CEffectObject::Bind_ShaderResource_Meshes()
{
    Get_Component<CShader>()->Set_Pass(0);
}

void CEffectObject::Bind_ShaderResource_Trails()
{
    Get_Component<CShader>()->Set_Pass(0);
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
}
