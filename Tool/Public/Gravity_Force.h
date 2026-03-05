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

class CGravity_Force :
    public Tool_PartObject
{
public:
    using Super = Tool_PartObject;

protected:
    CGravity_Force(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CGravity_Force(const CGravity_Force& rhs);
    virtual ~CGravity_Force() = default;

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
public:
    static CGravity_Force* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END