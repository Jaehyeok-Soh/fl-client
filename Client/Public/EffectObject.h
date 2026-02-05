#pragma once
#include "PartObject.h"
#include "Client_Defines.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Client)

class CEffectObject :
    public CPartObject
{
public:
    using Super = CPartObject;
    using _uint2 = struct { _uint x; _uint y; };

public:
    typedef struct tagEffectObjectDesc : public Super::PARTOBJ_DESC
    {
        // ========     이펙트 타입   =========
        DTO::E_EffectSystemType eEffectSystemType = DTO::E_EffectSystemType::Particle;
        DTO::E_PARTICLETYPE eEffectParticleType = DTO::E_PARTICLETYPE::PARTICLE;
        DTO::E_EFFECTTYPE eEffectType = DTO::E_EFFECTTYPE::Particle;
        DTO::E_SHAPETYPE _Effect_ShapeType = DTO::E_SHAPETYPE::SPREAD;

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
        int                 _Effect_MaxParticle = { 100 };

        // ========  이펙트 Radius  ==========
        Vec3                _Effect_Range = { 1.f, 1.f, 1.f };

        // ========  이펙트 Texture Flag  =======
        _uint               _Effect_TextureFlag = {};
        _uint               _Effect_RenderFlag = {};
        _uint               _Effect_SamplerStateFlag = {};
        _uint               _Effect_TextureRotationFlag = {};
        _uint               _Effect_TextureOperatorFlag = {};
    }Effect_Desc;

protected:
    CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CEffectObject(const CEffectObject& rhs);
    virtual ~CEffectObject() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual HRESULT Ready_Component(void* pArg);
    virtual HRESULT Ready_Component_Shader();
    virtual HRESULT Ready_Component_Texture();
    virtual HRESULT Ready_Component_Buffer(void* pArg);
    virtual HRESULT Ready_Component_Model(void* pArg);

public:
    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update_Priority(const _float fDT) override;
    virtual void Update(const _float fTimeDelta) override;
    virtual void Update_Late(const _float fTimeDelta) override;
    virtual void Ready_Before_Render(const _float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual _bool Picking(OUT Vec3& vOut) override;
    virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
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
private:
    void TimeCalculate(const _float fDT);
public:
    void TimeReset(Effect_Desc Desc);

public:
    const DTO::E_EffectSystemType& Get_EffectType() { return m_tEffectDesc.eEffectSystemType; }
    const Effect_Desc& Get_EffectDesc() { return m_tEffectDesc; }

public:
    static CEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    //  ========== 이펙트 Desc ===========
    Effect_Desc        m_tEffectDesc = {};
    Effect_Desc        m_tPrevEffectDesc = {};

    //  ========== 스크롤 OffSet ========
    Vec2      m_vScrollOffset = { 0.f, 0.f };
    _float    m_fTimeAccumulation = 0.f;
    _bool     m_bIsStarted = { false }; 

    //  ========== 현재 이펙트 sprite Number  ===========
private:
    _bool              m_bIsTool = { false };
    string             m_szName = {};
};

NS_END