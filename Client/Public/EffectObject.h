#pragma once
#include "EffectPartBase.h"
#include "Client_Defines.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CModel;
class CTexture;
class CShader;
class CComputeShader;
class CTransform;
class StructuredBuffer;
class CVIBuffer_Particle;
    NS_END

NS_BEGIN(Client)

class CEffectObject :
    public CEffectPartBase
{
public:
    using Super = CEffectPartBase;

    typedef struct tagEffectObjectDesc : public Super::PARTOBJ_DESC
    {
        DTO::TEFFECT_PartsData Data;
    }Effect_Desc;
  
protected:
    CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CEffectObject(const CEffectObject& rhs);
    virtual ~CEffectObject() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    HRESULT EffectDesc_Initialize(void* pArg);
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

public:
        // 외부 호출 함수
    virtual HRESULT Spawn_FromPool(void* pArg);
    virtual HRESULT Despawn_FromPool();

    virtual HRESULT Enable_VFX(void* pArg) override;
    virtual HRESULT Disable_VFX() override;

    virtual void LoopState_Change(DTO::E_LoopState eState) override;

public:
    _bool IsEffectfinish() {return m_bIsEffectFinish;}

private:
    //  ==========  Shader Binding Setting  =============
    HRESULT Bind_ShaderResource();
    HRESULT Bind_Curve_To_GPU();    // GPU에게 연산시키기.
private:
    // ====== 계산함수 ====== 
    void TimeCalculate(const _float fDT);
    _bool Is_Render_Possible() { return m_bIsStarted; }
    void Update_Gravity_Force(); // 중력 계산하기.
    float Sample_RotationCurve(const vector<DTO::Rotation_CurveKey>& vecCurve, float fLifeRatio);
    void Update_Rotation_Lerp(float fDT, float fRatio);
    void Update_UV_Scroll_Curve(float fRatio);
    void Apply_Scaling_Dynamics(const _float fRatio);

public:
    void TimeFlagRequest(_uint iTimeFlag);
    void RESET_ForSpawn();
    void RESET_ForDesPawn();
    HRESULT Process_InitializeDesc(void* pArg);

public:
    const DTO::E_EffectSystemType& Get_EffectType() { return (DTO::E_EffectSystemType)m_tEffectDesc.Data.eEffectSystemType; }
    const DTO::TEFFECT_PartsData& Get_EffectDesc() { return m_tEffectDesc.Data; }

private:


public:
    static CEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    //  ========== 이펙트 Desc ===========
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
    ID3DX11EffectShaderResourceVariable* pSRV = nullptr;        // GPU 중력 계산 보낼 것.
   StructuredBuffer*              pSB = nullptr;         

    _bool              m_bIsTool = { false };

private:
    // 캐싱 용도
   CModel*                   m_pModel = { nullptr };
   CTexture*                 m_pTexture = { nullptr };
   CShader*                  m_pShader = { nullptr };
   CComputeShader*           m_pComputeShader = { nullptr };
   CTransform*               m_pTransform = { nullptr };
   CVIBuffer_Particle*       m_pParticleBuffer = { nullptr };

   vector<_uint>             m_iSpriteCurrentNumber = {};
   vector<float>             m_iSpriteAccumulation = {};

 private:
   _bool                     m_bIsEffectFinish = { false };
   _bool                     m_bDespawnFlag = { false };
};

NS_END