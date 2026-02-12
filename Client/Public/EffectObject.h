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
    virtual _bool Picking(OUT Vec3& vOut) override;
    virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
    virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
        // 외부 호출 함수
    virtual HRESULT Spawn_FromPool(void* pArg);
    virtual HRESULT Despawn_FromPool();

private:
    //  ==========  Shader Binding Setting  =============
    HRESULT Bind_ShaderResource();
private:
    // ====== 계산함수 ====== 
    void TimeCalculate(const _float fDT);
    float Sample_GravityCurve(const vector<DTO::Gravity_CurveKey>& vecVurve, float fLifeRatio);
    void Update_Gravity_Force(float fLifeRatio); // 중력 계산하기.
    float Sample_RotationCurve(const vector<DTO::Rotation_CurveKey>& vecCurve, float fLifeRatio);
    void Update_Rotation_Lerp(float fDT, float fRatio);
    void Update_UV_Scroll_Curve(float fRatio);

public:
    void TimeFlagRequest(_uint iTimeFlag);

public:
    const DTO::E_EffectSystemType& Get_EffectType() { return (DTO::E_EffectSystemType)m_tEffectDesc.Data.eEffectSystemType; }
    const DTO::TEFFECT_PartsData& Get_EffectDesc() { return m_tEffectDesc.Data; }

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
    _bool     m_bIsStarted = { false }; // 타임 딜레이 지났는지에 대한 bool값

    //  ========= 회전 속도 ===========
    Vec3       m_vAccumulatedRotation = { 0.f, 0.f, 0.f };

    //  ========== 현재 이펙트 sprite Number  ===========
private:
    _bool              m_bIsTool = { false };
};

NS_END