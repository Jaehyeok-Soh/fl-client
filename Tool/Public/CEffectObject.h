#pragma once
#include "ToolObject.h"
#include "EffectType_Selection_Panel.h"

NS_BEGIN(Tool)

enum class E_EffectType
{
    None,
    Particle,
    Mesh,
    Trail,
};

enum class E_EffectTextureType
{
    DIFFUSE = 0,
    NOISE = 1,
};

class CEffectObject :
    public CToolObject
{
public:
    using Super = CToolObject;
    using _uint2 = struct { _uint x; _uint y; };

public:
    typedef struct tagEffectObjectDesc : public Super::TOOLOBJECT_DESC
    {
        // ========     ¿Ã∆Â∆Æ ≈∏¿‘   =========
        E_EffectSystemType eEffectSystemType = E_EffectSystemType::None;
        E_EffectType eEffectType = E_EffectType::None;


        // ========  ¿Ã∆Â∆Æ Material º≥¡§   ===========
        wstring     _Effect_Model_Tag = {};
        wstring     _Effect_Shader_Tag = {};
        wstring     _Effect_DiffuseTexture_Tag = {};
        wstring     _Effect_Mesh_NoiseTexture_Tag = {};
        _uint       _Effect_ShaderPass = {};

        // =======   ¿Ã∆Â∆Æ Ω∫≈©∑— Value   ===========
        _float2     _Effect_ScrollSpeed = { 0.f, 0.f };
        _uint       _Effect_fameSpeed = {};

        // ========   ¿Ã∆Â∆Æ ø÷∞Ó Scale Value   ==========
        _float2     _Effect_DistortionScale = { 0.f, 0.f };

        // ==========   ¿Ã∆Â∆Æ Sacle Value   ==============
        _float3     _Effect_StartScale = { 1.f, 1.f, 1.f };
        _float3     _Effect_EndScale = { 1.f, 1.f, 1.f };

        // =========   ¿Ã∆Â∆Æ Color Value   ===============
        _float4     _Effect_Color = { 0.f, 0.f, 0.f, 0.f };

        // =========   ¿Ã∆Â∆Æ Atlas Texture ¿¸øÎ  =========
        bool        _Effect_bUseAtlas = {};
        _uint2      _Effect_TileCount = {};
    }Effect_Desc;

protected:
    CEffectObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CEffectObject(const CToolObject& rhs);
    virtual ~CEffectObject() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update_Priority(const _float fDT) override;
    virtual void Update(const _float fTimeDelta) override;
    virtual void Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual _bool Picking(OUT _float4& vOut) override;
    virtual HRESULT Export_Data(OUT MAPOBJECT_SAVEDATA& data) PURE;
    virtual void Draw_ImGui() override;
    virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
    HRESULT EffectDesc_Initialize(void* pArg);

public:
    const E_EffectSystemType& Get_EffectType() { return m_tEffectDesc.eEffectSystemType; }
    const Effect_Desc& Get_EffectDesc() { return m_tEffectDesc; }

    void Set_EffectDesc(const Effect_Desc& Desc) { m_tEffectDesc = Desc; }

public:
    static CEffectObject* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual void Free() override;

private:
    Effect_Desc        m_tEffectDesc = {};
};

NS_END