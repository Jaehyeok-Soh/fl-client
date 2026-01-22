#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

enum class E_EffectSystemType
{
    None, Particle, ForceField, Line, Trail
};

class CEffectType_Selection_Panel final :
    public CImGui_Panel
{
    using Super = CImGui_Panel;

protected:
    explicit CEffectType_Selection_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CEffectType_Selection_Panel() = default;

    HRESULT EffectPanel_Initialize();

public:
    void Update(const _float fDT) override;
    virtual HRESULT Render(CToolObject* pGo) override;
    

public:
    // 기본 생성창
    void DrawEmbedded();

    // ==========   Effect Type 별 생성 함수  =====================
    void CreateParticleEffect();
    void CreateForceFieldEffect();
    void CreateLineEffect();
    void CreateTrailEffect();

public:
    virtual void Free() override;
    static CEffectType_Selection_Panel* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
    E_EffectSystemType  m_eSelectedEffectType = E_EffectSystemType::None;
    CGameInstance*      m_pGameInstance = { nullptr };
};

NS_END