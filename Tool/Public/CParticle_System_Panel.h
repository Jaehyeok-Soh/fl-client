#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CParticle_System_Panel :
    public CImGui_Panel
{
    using Super = CImGui_Panel;

protected:
    explicit CParticle_System_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CParticle_System_Panel() = default;

protected:
    HRESULT EffectPanel_Initialize();

public:
    void Update(const _float fDT) override;
    virtual HRESULT Render(CToolObject* pGo) override;

public:
    virtual void Free() override;
    static CParticle_System_Panel* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);


private:
    CGameInstance* m_pGameInstance = { nullptr };
};

NS_END