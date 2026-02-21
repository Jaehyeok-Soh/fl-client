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
enum class E_TEXTURETYPE {
    DIFFUSE = 0,
    NOISE = 1,
    MASKING = 2,
    GRADATION = 3,
    TRAIL = 4,
    NORMAL = 5,
    GLOW = 6,
    DISSOLVE = 7,
};
enum class E_EffectSystemType { NONE = 0, Particle, ForceField, Line, Trail };
enum class E_SHAPETYPE {
    NONE = 0,
    DROP,
    RISE,
    SPREAD,
    STRAIGHT,
    SPIRAL,
    DNA,
    GATHER,   // 중앙으로 모이기
    FOUNTAIN  // 분수 효과
};
enum class E_SIMULATION_SPACE { NONE = 0, LOCAL, WORLD };
enum class E_EMISSION_TYPE { BOX = 0, CIRCLE, SPHERE, CONE };
enum class EEffectType : _uint{ EFFECT_CONTAINER, EFFECT_PARTS, END};
inline constexpr _uint g_EffectTypeCount{ ENUM_TO_UINT(EEffectType::END) };

struct Gravity_CurveKey 
{
    float fTimeKey = { 0.f };
    float fValue = { 0.f };
};

struct Rotation_CurveKey
{
    float fTimeKey = { 0.f };
    float fValue = { 0.f };
};

enum E_RANDOM_FLAG {
    RAND_NONE = 0,
    RAND_POS = 1 << 0,
    RAND_LIFE = 1 << 1,
    RAND_SIZE = 1 << 2
};

// 비트 연산자 오버로딩
inline E_RANDOM_FLAG operator|(E_RANDOM_FLAG a, E_RANDOM_FLAG b) { return static_cast<E_RANDOM_FLAG>(static_cast<int>(a) | static_cast<int>(b)); }
inline E_RANDOM_FLAG operator&(E_RANDOM_FLAG a, E_RANDOM_FLAG b) { return static_cast<E_RANDOM_FLAG>(static_cast<int>(a) & static_cast<int>(b)); }
inline E_RANDOM_FLAG operator~(E_RANDOM_FLAG a) { return static_cast<E_RANDOM_FLAG>(~static_cast<int>(a)); }
inline E_RANDOM_FLAG& operator|=(E_RANDOM_FLAG& a, E_RANDOM_FLAG b) { return a = a | b; }
inline E_RANDOM_FLAG& operator&=(E_RANDOM_FLAG& a, E_RANDOM_FLAG b) { return a = a & b; }

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
    _uint _Effect_EmissionType = ENUM_TO_UINT(E_EMISSION_TYPE::BOX);

    // ========  이펙트 Material 설정   ===========
    wstring     _Effect_Model_Tag = {};
    wstring     _Effect_DiffuseTexture_Tag = {};
    wstring     _Effect_NoiseTexture_Tag = {};
    wstring     _Effect_MaskingTexture_Tag = {};
    wstring     _Effect_GradationTexture_Tag = {};
    wstring     _Effect_TrailTexture_Tag = {};
    wstring     _Effect_NormalTexture_Tag = {};
    wstring     _Effect_DissolveTexture_Tag = {};
    wstring     _Effect_GlowTexture_Tag = {};

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

    // =========   시간 관련 값  ================
        // 0 : Play, 1 : Pause,  2: Reset, 3:  Stop
    _uint               _Effect_TimeFlag = {};
    _float              _Effect_StartDelay = { 0.f };
    _float              _Effect_LifeTime = { 5.f };
    _float              _Effect_ApearRatio = { 0.f };

    // =========   이펙트 Sprite 사용 여부    ============
    bool                 _Effect_bUseSprite = {};
    _uint2               _Effect_TileCount = {};
    bool                 _Effect_bPlayAnim = { false };
    _float               _Effect_AnimSpeed = { 1.0f };
    _uint                m_iCurSpriteNumber = {};


    // =========   이펙트 Emission 전용   =============
    _float      _Effect_RateOverTime = {};
    _float      _Effect_RateOverDistance = {};


    // ========   이펙트 파티클 전용   ============
    Vec2                _Effect_ParticleSize = { 0.05f, 0.15f };
    _float              _Effect_Duration = { 5.f };
    _bool               _Effect_Looping = { true };
    E_RANDOM_FLAG       iRandomFlags = RAND_NONE;

    _float              _Effect_PlayBackSpeed = { 1.f };
    _float              _Effect_StartSpeed = { 1.f };   // Particle에 영향을 주는 스피드 [개별 배속]
    int                 _Effect_MaxParticle = { 30 };

    // ========  이펙트 Radius  ==========
    Vec3                _Effect_Range = { 1.f, 1.f, 1.f };
    float               _Effect_Spiral_Radius = { 1.f };
    float               _Effect_Spiral_Speed = { 1.f };

    // ========  이펙트 UV Offset  ==========
    Vec2               _Effect_UV_Offset = { 0.f, 0.f };

    // ==============  중력 값들   ==============
    // Base Data
    float               _Effect_Gravity_Value = { 9.8f };           // 물리적 기준값
    float               _Effect_GravityModifier = { 0.f };          // 전체적인 On / off 기능
    Vec3                _Effect_GravityDir = { 0.f, -1.f, 0.f };    // 중력 방향

    // ==============  중력 커브  ==============
    bool                     _bUseGlobalGravityCurve = false;   // 커브 사용 여부 플래그
    vector<Gravity_CurveKey> _vecGlobalGravityCurve;  // 시간대별 0.0~1.0 비율값

    bool                     _bUseExternalForceCurve = false;   // 외부 중력(Force Field) 커브 여부
    float                    fExternalForceStrength = 1.0f; // 커브 안 쓸 때 기본값
    vector<Gravity_CurveKey> _vecExternalForceCurve;  // 외부 중력용 시간대별 비율값 

    // =============  스크롤 커브  =============
    bool                     _bUseUVScrollCurve = false;
    vector<Rotation_CurveKey> _vecUVScrollCurveX;
    vector<Rotation_CurveKey> _vecUVScrollCurveY;

    // ==============  회전 값들   ==============
   // 3D Start Rotation 값
    Vec3                    _Effect_StartRotation = { 0.f, 0.f, 0.f };    // 초기 회전각을 얼마로 고정할건데?
    Vec3                    _Effect_TargetRotation = { 0.f, 0.f, 0.f }; // 얼만큼 회전시킬건데?
    bool                    _bUseStartRotation = false;
    // X,Y,Z 축 분리
    vector<Rotation_CurveKey> _vecRotationCurveX;
    vector<Rotation_CurveKey> _vecRotationCurveY;
    vector<Rotation_CurveKey> _vecRotationCurveZ;

    bool _bUseRotationCurve = false;
    bool _bSeparateAxes = false;

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
    _bool               _Effect_Tool_DissolveTexture = { false };
    _bool               _Effect_Tool_GlowTexture = { false };

    // 빌보드는 있니, 스크롤은 먹이니
    _bool               _Effect_Tool_UseBillboard = { false };
    _bool               _Effect_Tool_UseDirBillboard = { false };
    _bool               _Effect_Tool_UseScroll = { false };
    _bool               _Effect_Tool_RightScroll = { false };
    _bool               _Effect_Tool_DownScroll = { false };

    // + 텍스처별 스크롤 값 적용
    Vec2                _Effect_DiffuseTexture_ScrollWeight = { 1.f, 1.f };
    Vec2                _Effect_NoiseTexture_ScrollWeight = { 1.f, 1.f };
    Vec2                _Effect_MaskingTexture_ScrollWeight = { 1.f, 1.f };
    Vec2                _Effect_GradationTexture_ScrollWeight = { 1.f, 1.f };
    Vec2                _Effect_DissolveTexture_ScrollWeight = { 1.f, 1.f };

    // 툴용 텍스처 스크롤 

    _bool               _Effect_Tool_UseScroll_Diffuse = { false };
    _bool               _Effect_Tool_UseScroll_Noise = { false };
    _bool               _Effect_Tool_UseScroll_Masking = { false };
    _bool               _Effect_Tool_UseScroll_Gradation = { false };
    _bool               _Effect_Tool_UseScroll_Dissolve = { false };
    _bool               _Effect_Tool_UseScroll_Glow = { false };

    // SamplerState 몇번 쓸거니
    int               _Effect_Tool_DiffuseSamplerState_Flag = {};
    int               _Effect_Tool_NoiseSamplerState_Flag = {};
    int               _Effect_Tool_MaskingSamplerState_Flag = {};
    int               _Effect_Tool_GradationSamplerState_Flag = {};
    // 
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
    {EEffectType::EFFECT_CONTAINER, "EFFECT_CONTAINER"},
    {EEffectType::EFFECT_PARTS, "EFFECT_PARTS"},
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
