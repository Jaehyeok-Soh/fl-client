#pragma once
#include "Tool_PartObject.h"
#include "EffectType_Selection_Panel.h"

NS_BEGIN(Engine)

class CComponent;
class CTexture;
class CModel;
class CShader;

NS_END

NS_BEGIN(Tool)

enum class E_EFFECTTYPE
{
    NONE = 0 ,
    Particle,
    Mesh,
    Trail,
};

enum class TEXTURETYPE
{
    DIFFUSE = 0,
    NOISE = 1,
    MASKING = 2,
    GRADATION = 3,
    TRAIL = 4,
    NORMAL = 5,
};

enum class E_SHAPETYPE
{
    NONE = 0,
    SPREAD,
    DROP,
    RISE,
    MESH,
    STRAIGHT,
};

enum class E_RENDER_TYPE
{
    NONE = 0,
    BILBOARD,
    NONE_BILBOARD,
};

enum class E_PARTICLETYPE
{
    NONE = 0,
    PARTICLE,
    TEXTURE,
    MESH,
};

enum class E_SAMPLERSTATE_FLAG
{
    LinearSampler,
    LinearClampSampler,
    LinearBorderSampler,
    LinearMirrorSampler,
    PointSampler
};

enum class E_RASTERIZESTATE_FLAG
{
    RS_Default,
    RS_Default_CullFront,
    RS_Default_CullNone,
    RS_Wire,
};

enum class E_BLENDSTATE_FLAG
{
    BS_Default,
    BS_AlphaBlend,
    BS_Blend,
};

enum class E_DEPTHSTENCILSTATE_FLAG
{
    DS_Default,
    DS_Disabled,
    DS_ReadOnly,
    DS_Write,
};

// 비트 플래그로 파티클 종류 지정하기.
// 비트 플래그로 로컬 좌표 따라갈것인지, 월드 좌표 따라갈 것인지 정하기.

class CEffectObject :
    public Tool_PartObject
{
public:
    using Super = Tool_PartObject;
    using _uint2 = struct { _uint x; _uint y; };

public:
    typedef struct tagEffectObjectDesc : public Super::PARTOBJ_DESC
    {
        // ========     이펙트 타입   =========
        E_EffectSystemType eEffectSystemType = E_EffectSystemType::Particle;
        E_PARTICLETYPE eEffectParticleType = E_PARTICLETYPE::PARTICLE;
        E_EFFECTTYPE eEffectType = E_EFFECTTYPE::Particle;
        E_SHAPETYPE _Effect_ShapeType = E_SHAPETYPE::SPREAD;

        // ========  이펙트 Material 설정   ===========
        wstring     _Effect_Model_Tag = {};
        wstring     _Effect_DiffuseTexture_Tag = {};
        wstring     _Effect_NoiseTexture_Tag = {};
        wstring     _Effect_MaskingTexture_Tag = {};
        wstring     _Effect_GradationTexture_Tag = {};
        wstring     _Effect_TrailTexture_Tag = {};
        wstring     _Effect_NormalTexture_Tag = {};

        wstring     _Effect_Shader_Path = {};
        wstring     _Effect_Shader_Tag = {};
        int         _Effect_ShaderPass = { 0 };

        // =======   이펙트 스크롤 Value   ===========
        Vec2     _Effect_ScrollSpeed = { 0.f, 0.f };

        // ========   이펙트 왜곡 Scale Value   ==========
        Vec2     _Effect_DistortionScale = { 0.f, 0.f };

        // ==========   이펙트 Sacle Value   ==============
        Vec3     _Effect_StartScale = { 1.f, 1.f, 1.f };
        Vec3     _Effect_EndScale = { 1.f, 1.f, 1.f };

        // =========   이펙트 Color Value   ===============
        Vec4     _Effect_Color = { 1.f, 0.f, 0.f, 1.f };
        float    _Effect_DiscardValue = { 0.05f };

        // =========   Tool용 시간 값   ================
        bool      _Effect_TimeStop = true;

        // =========   이펙트 Sprite 사용 여부    ============
        bool        _Effect_bUseSprite = {};
        _uint2      _Effect_TileCount = {};
        bool        _Effect_bPlayAnim = { false };
        _float      _Effect_AnimSpeed = { 1.0f };
        _uint       m_iCurSpriteNumber = {};


        // =========   이펙트 Emission 전용   =============
        _float      _Effect_RateOverTime = {};
        _float      _Effect_RateOverDistance = {};

        // ========   이펙트 파티클 전용   ============
        Vec2                _Effect_ParticleSize = { 0.05f, 0.15f };
        _float              _Effect_Duration = { 5.f };
        _bool               _Effect_Looping = { true };
        _bool               _Effect_IsRandomSeed = { true };
        _float              _Effect_StartDelay = { 0.f };
        _float              _Effect_LifeTime = { 5.f };
        _float              _Effect_PlayBackSpeed = { 1.f };
        _float              _Effect_StartSpeed = { 1.f };   // Particle에 영향을 주는 스피드 [개별 배속]
        int                 _Effect_MaxParticle = { 30 };

        // ========  이펙트 Radius  ==========
        Vec3                _Effect_Range = { 1.f, 1.f, 1.f };

        // ========  이펙트 Texture Flag  =======
        _uint               _Effect_TextureFlag = {};
        _uint               _Effect_RenderFlag = {};
        _uint               _Effect_SamplerStateFlag = {};
        _uint               _Effect_TextureRotationFlag = {};
        _uint               _Effect_TextureOperatorFlag = {};

        // ========  툴용 Flag ========
        // Texture 쓰니?
        _bool               _Effect_Tool_DiffuseTexture = { false };
        _bool               _Effect_Tool_NoiseTexture = { false };
        _bool               _Effect_Tool_MaskingTexture = { false };
        _bool               _Effect_Tool_GradationTexture = { false };

        // 빌보드는 있니, 스크롤은 먹이니
        _bool               _Effect_Tool_UseBillboard = { false };
        _bool               _Effect_Tool_UseScroll = { false };
        _bool               _Effect_Tool_RightScroll = { false };
        _bool               _Effect_Tool_DownScroll = { false };

        // SamplerState 몇번 쓸거니
        int               _Effect_Tool_DiffuseSamplerState_Flag = {};
        int               _Effect_Tool_NoiseSamplerState_Flag = {};
        int               _Effect_Tool_MaskingSamplerState_Flag = {};
        int               _Effect_Tool_GradationSamplerState_Flag = {};

        // 
    }Effect_Desc;

protected:
    CEffectObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CEffectObject(const CEffectObject& rhs);
    virtual ~CEffectObject() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update_Priority(const _float fDT) override;
    virtual void Update(const _float fTimeDelta) override;
    virtual void Update_Late(const _float fTimeDelta) override;
    virtual void Ready_Before_Render(const _float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual _bool Picking(OUT Vec3& vOut) override;
    virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
    virtual void Draw_ImGui() override;
    virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
    //  ==========  초기 Component 설정  ================
    HRESULT EffectDesc_Initialize(void* pArg);
    HRESULT Component_Setting(void* pArg);

    void Model_Setting(const wstring& Name);
    void Shader_Setting(const wstring& Name);
    void Texture_Setting(const wstring& Name);
    void Buffer_Setting();

    void Particle_Setting();

private:
    //  ==========  Shader Binding Setting  =============
    HRESULT Bind_ShaderResource();

    void Bind_ShaderResource_Particles();
    void Bind_ShaderResource_Meshes();
    void Bind_ShaderResource_Trails();

private:
    void TimeCalculate(const _float fDT);
public:
    void TimeReset(Effect_Desc Desc);
    void TimePause(_bool b) { m_tEffectDesc._Effect_TimeStop = b; }

public:
    const E_EffectSystemType& Get_EffectType() { return m_tEffectDesc.eEffectSystemType; }
    const Effect_Desc& Get_EffectDesc() { return m_tEffectDesc; }

    void Set_EffectDesc(const Effect_Desc& Desc);

public:
    // 툴 전용 - Preview Texture Effect 전용
    void Preview_Texture_Reset();
    void Preview_TextureKey_Binding(const string& Key);


public:
    static CEffectObject* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    //  ========== 이펙트 Desc ===========
    Effect_Desc        m_tEffectDesc = {};
    Effect_Desc        m_tPrevEffectDesc = {};

    //  ========== 스크롤 OffSet ========
    Vec2      m_vScrollOffset = { 0.f, 0.f };
    _float    m_fTimeAccumulation = 0.f;
    _bool  m_bIsStarted = { false }; // 타임 딜레이 지났는지에 대한 bool값

    //  ========== 현재 이펙트 sprite Number  ===========
private:
    _bool              m_bIsTool = { false };
};

NS_END