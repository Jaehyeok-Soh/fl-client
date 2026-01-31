#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  EFFECT  -------------------/////////////////
// 선언부
using _uint2 = struct { _uint x; _uint y; };

enum class E_EFFECTTYPE { NONE = 0, Particle, Mesh, Trail };
enum class E_RENDER_TYPE { NONE = 0, BILBOARD, NONE_BILBOARD };
enum class E_PARTICLETYPE { NONE = 0, PARTICLE, TEXTURE, MESH };
enum class E_SAMPLERSTATE_FLAG { LinearSampler, LinearClampSampler, LinearBorderSampler, LinearMirrorSampler, PointSampler };
enum class E_TEXTURETYPE { DIFFUSE = 0, NOISE, MASKING, GRADATION, TRAIL, NORMAL };
enum class E_EffectSystemType { NONE = 0, Particle, ForceField, Line, Trail };
enum class E_SHAPETYPE { NONE = 0, SPREAD, DROP, RISE, MESH, STRAIGHT };
enum class E_SIMULATION_SPACE { NONE = 0, LOCAL, WORLD };
enum class EEffectType : _uint{ EFFECT_CONTAINER, EFFECT_PARTS, END};
inline constexpr _uint g_EffectTypeCount{ ENUM_TO_UINT(EEffectType::END) };


struct TEFFECT_PartsData
{
    static constexpr EEffectType eType = EEffectType::EFFECT_PARTS;
    std::string strTag{ "EffectObject" };
    std::string EffectPartsName = {};
    std::string ParentsName = {};

    Matrix      vWorldMatrix = {};

    _uint eEffectSystemType = ENUM_TO_UINT(E_EffectSystemType::Particle);
    _uint eEffectParticleType = ENUM_TO_UINT(E_PARTICLETYPE::PARTICLE);
    _uint eEffectType = ENUM_TO_UINT(E_EFFECTTYPE::Particle);
    _uint _Effect_ShapeType = ENUM_TO_UINT(E_SHAPETYPE::SPREAD);

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
    int                 _Effect_MaxParticle = { 100 };

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
};

struct TEFFECT_ContainerData
{
    static constexpr EEffectType eType = EEffectType::EFFECT_CONTAINER;
    std::string strTag{ "Effect" };
    std::string EffectContainerName = {};

    Matrix      vWorldMatrix = {};
    _uint _Effect_SimulationType = ENUM_TO_UINT(E_SIMULATION_SPACE::NONE);
    vector<TEFFECT_PartsData>   _ChildData = {};
};

NLOHMANN_JSON_SERIALIZE_ENUM(EEffectType,
	{
		{EEffectType::END, "END"}
	}
)

/////////////////-------------------  to_json, from_json  -------------------/////////////////
inline void to_json(json& j, const TEFFECT_ContainerData& data);
inline void from_json(const json& j, TEFFECT_ContainerData& data);

inline void to_json(json& j, const TEFFECT_PartsData& data);
inline void from_json(const json& j, TEFFECT_PartsData& data);

NS_END


NS_BEGIN(Engine)

// ============  Effect 상위 객체  ==================
class ENGINE_DLL CEFFECT_CONTAINER final : public IObjectDataBase
{
    using Super = IObjectDataBase;
private:
    CEFFECT_CONTAINER() = default;
    virtual ~CEFFECT_CONTAINER() = default;

public:
    _uint Get_Type() const override { return ENUM_TO_UINT(DTO::EEffectType::EFFECT_CONTAINER); }
    const _string& Get_Tag() const override { return m_Data.strTag; }

    json ToJson() const override;
    HRESULT FromJson(const json& j) override;

    const DTO::TEFFECT_ContainerData& Get_Data() const { return m_Data; }
    DTO::TEFFECT_ContainerData& Get_Data() { return m_Data; }

private:
    DTO::TEFFECT_ContainerData m_Data;

public:
    static CEFFECT_CONTAINER* Create() { return new CEFFECT_CONTAINER(); }
    virtual void Free() override { Super::Free(); }
};

NS_END
