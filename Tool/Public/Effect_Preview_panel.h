#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CEffect_Preview_panel :
    public CImGui_Panel
{
    using Super = CImGui_Panel;
protected:
    explicit CEffect_Preview_panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CEffect_Preview_panel() = default;

public:
    void Update(const _float fDT) override;
    virtual HRESULT Render(CToolObject* pGo) override;

public:
    virtual void Free() override;
    static CEffect_Preview_panel* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
    void Draw_PreviewActor(CToolObject* pGo);
public:
    CGameInstance* m_pGameInstance = { nullptr };
};

NS_END