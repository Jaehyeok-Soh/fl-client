#pragma once
#include "Tool_PartObject.h"
#include "EffectType_Selection_Panel.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CComponent;
class CTexture;
class CModel;
class CShader;

NS_END

NS_BEGIN(Tool)

enum class TEXTURE_INFO
{
    DEFAULTTEXTURE = 0,
    NOISETEXTURE  = 1,
    MASKINGTEXTURE  = 2,
    GRADATIONTEXTURE  = 3,
    TRAILTEXTURE  = 4,
    NORMALTEXTURE = 5,
    GLOWTEXTURE  = 6,
    DISSOLVETEXTURE  = 7,
    CURVETEXTURE  = 8,
    END
};

class CEffectObject :
    public Tool_PartObject
{
public:
    using Super = Tool_PartObject;
public:
    typedef struct tagEffectObjectDesc : public Super::PARTOBJ_DESC
    {
        DTO::TEFFECT_PartsData Data;
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

    bool IntsersectWithPlane(OUT Vec3& vOut);
public:
    //  ==========  초기 Component 설정  ================
    HRESULT Ready_Component(void* pArg);
    HRESULT Ready_Component_Shader();
    HRESULT Ready_Component_Texture();
    HRESULT Ready_Component_Model(void* pArg);
    HRESULT Ready_Component_Buffer(void* pArg);

    void Model_Setting(const wstring& Name);
    void Shader_Setting(const wstring& Name);
    void Texture_Setting(const wstring& Name);
    void Buffer_Setting();

    void Particle_Setting();

public:
    // 외부 호출 함수
    virtual HRESULT Spawn_FromPool(void* pArg);
    virtual HRESULT Despawn_FromPool();

public:
    _bool IsEffectfinish() { return m_bIsEffectFinish; }

private:
    //  ==========  Shader Binding Setting  =============
    HRESULT Bind_ShaderResource();
    HRESULT Bind_Curve_To_GPU();

private:
    // ====== 계산함수 ====== 
    void TimeCalculate(const _float fDT);
    //float Sample_GravityCurve(const vector<DTO::Gravity_CurveKey>& vecVurve, float fLifeRatio);   // 이제 GPU에서
    void Update_Gravity_Force(); // 중력 계산하기.
    float Sample_RotationCurve(const vector<DTO::Rotation_CurveKey>& vecCurve, float fLifeRatio);
    void Update_Rotation_Lerp(float fDT, float fRatio);
    void Update_UV_Scroll_Curve(float fRatio);

public:
    void TimeFlagRequest(_uint TimeFlag);

public:
    const DTO::E_EffectSystemType& Get_EffectType() { return (DTO::E_EffectSystemType)m_tEffectDesc.Data.eEffectSystemType; }
    const DTO::TEFFECT_PartsData& Get_EffectDesc() { return m_tEffectDesc.Data; }

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
    ID3DX11EffectShaderResourceVariable* pSRV = nullptr;
    StructuredBuffer* pSB = nullptr;

    Effect_Desc        m_tEffectDesc = {};
    Effect_Desc        m_tPrevEffectDesc = {};

    Effect_Desc        m_tOriginEffectDesc = {};
    //  ========== 스크롤 OffSet ========
    Vec2      m_vScrollOffset = { 0.f, 0.f };
    _float    m_fTimeAccumulation = 0.f;
    _bool     m_bIsStarted = { false }; // 타임 딜레이 지났는지에 대한 bool값

    //  ========= 회전 속도 ===========
    Vec3      m_vAccumulatedRotation = { 0.f, 0.f, 0.f };
    Vec3      m_vFinalGravity = { 0.f, 0.f, 0.f }; 

    //  ========== 현재 이펙트 sprite Number  ===========
private:

    // 캐싱 용도
    CModel* m_pModel = { nullptr };
    CTexture* m_pTexture = { nullptr };
    CShader* m_pShader = { nullptr };
    CComputeShader* m_pComputeShader = { nullptr };
    CTransform* m_pTransform = { nullptr };
    _bool       m_bIsTool = { false };

    vector<_uint>           m_iSpriteCurrentNumber = {};

private:
    _bool       m_bIsEffectFinish = { false };
};

NS_END