#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolObject;

enum class E_EffectSystemType
{
    None, 
    Particle, 
    ForceField, 
    Line, 
    Trail
};

class CEffectType_Selection_Panel final :
    public CImGui_Panel
{
    using Super = CImGui_Panel;

protected:
    explicit CEffectType_Selection_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CEffectType_Selection_Panel() = default;

    HRESULT EffectPanel_Initialize(CToolObject** pTarget);

public:
    void Update(const _float fDT) override;
    virtual HRESULT Render(CToolObject* pGo) override;
    

public:
    // 기본 생성창
    void DrawEmbedded();

    // 내가 생성한 Effect List를 가지고 온다.
    void DrawEffectList();
    void EditEffect();
    // ==========   Effect Type 별 생성 함수  =====================
    void CreateParticleEffect();
    void CreateForceFieldEffect();
    void CreateLineEffect();
    void CreateTrailEffect();


protected:
    wstring TypeToString(E_EffectSystemType eType);

public:
    virtual void Free() override;
    static CEffectType_Selection_Panel* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CToolObject** pTarget);

private:
    E_EffectSystemType  m_eSelectedEffectType = E_EffectSystemType::None;
    CGameInstance*      m_pGameInstance = { nullptr };

    //  =========  Effect Scene에서 캐싱해온 변수  =============
    CToolObject** m_ppTargetSlot = { nullptr };

    //  =========  Effect List 담아올 컨테이너  ================
    std::vector<std::pair<string, CToolObject*>>     m_pEffectList = {};
};

NS_END