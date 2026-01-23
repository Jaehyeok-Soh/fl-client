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
};

enum class E_SHAPETYPE
{
    NONE = 0,
    SPHERE,
    HEMISPHERE,
    CONE,
    CIRCLE,
};

enum class E_SIMULATION_SPACE
{
    NONE = 0,
    LOCAL,
    WORLD,
};

enum class E_PARTICLETYPE
{
    NONE = 0,
    PARTICLE,
    TEXTURE,
    MESH,
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
        E_EffectSystemType eEffectSystemType      = E_EffectSystemType::None;
        E_PARTICLETYPE eEffectParticleType        = E_PARTICLETYPE::NONE;
        E_EFFECTTYPE eEffectType                  = E_EFFECTTYPE::NONE;
        E_SHAPETYPE _Effect_ShapeType             = E_SHAPETYPE::NONE;
        E_SIMULATION_SPACE _Effect_SimulationType = E_SIMULATION_SPACE::NONE;

        // ========  이펙트 Material 설정   ===========
        wstring     _Effect_Model_Tag = {};
        wstring     _Effect_Shader_Tag = {};
        wstring     _Effect_DiffuseTexture_Tag = {};
        wstring     _Effect_Mesh_NoiseTexture_Tag = {};
        _uint       _Effect_ShaderPass = {};

        // =======   이펙트 스크롤 Value   ===========
        Vec2     _Effect_ScrollSpeed = { 0.f, 0.f };

        // ========   이펙트 왜곡 Scale Value   ==========
        Vec2     _Effect_DistortionScale = { 0.f, 0.f };

        // ==========   이펙트 Sacle Value   ==============
        Vec3     _Effect_StartScale = { 1.f, 1.f, 1.f };
        Vec3     _Effect_EndScale = { 1.f, 1.f, 1.f };

        // =========   이펙트 Color Value   ===============
        Vec4     _Effect_Color = { 0.f, 0.f, 0.f, 0.f };

        // =========   Tool용 시간 값   ================
        _float      _Effect_ToolFlag = false;
        _float      _Effect_TimeAccumulation = 0.f;

        // =========   이펙트 Atlas Texture 전용  =========          << 거의 안쓸듯. 아틀라스 할 바에 편집하고 말지
        bool        _Effect_bUseAtlas = {};
        _uint2      _Effect_TileCount = {};

        // ========   이펙트 파티클 전용   ============
        _float      _Effect_Duration = { 0.f };
        _bool       _Effect_Looping = { false };
        _float      _Effect_StartDelay = { 0.f };
        _float      _Effect_LifeTime = { 0.f };
        _float      _Effect_StartSpeed = { 1.f };
        _uint       _Effect_MaxParticle = { 100 };
        _bool       _Effect_BillBoardFlag = { false };


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
    virtual HRESULT Export_Data(OUT MAPOBJECT_SAVEDATA& data) override;
    virtual void Draw_ImGui() override;
    virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
    //  ==========  초기 Component 설정  ================
    HRESULT EffectDesc_Initialize(void* pArg);
    HRESULT Component_Setting();

private:
    //  ==========  Shader Binding Setting  =============
    HRESULT Bind_ShaderResource();

    void Bind_ShaderResource_Particles();
    void Bind_ShaderResource_Meshes();
    void Bind_ShaderResource_Trails();

public:
    const E_EffectSystemType& Get_EffectType() { return m_tEffectDesc.eEffectSystemType; }
    const Effect_Desc& Get_EffectDesc() { return m_tEffectDesc; }

    void Set_EffectDesc(const Effect_Desc& Desc) { m_tEffectDesc = Desc; }

public:
    static CEffectObject* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    //  ========== 이펙트 Desc ===========
    Effect_Desc        m_tEffectDesc = {};

    //  ========== 이펙트 재료 ===========
    CModel*      m_pModelCom = { nullptr };
    CShader*     m_pShaderCom = { nullptr };
    CTexture*    m_pTextureCom = { nullptr };

    //  ========== 스크롤 OffSet ========
    Vec2      m_vScrollOffset = { 0.f, 0.f };
    
};

NS_END