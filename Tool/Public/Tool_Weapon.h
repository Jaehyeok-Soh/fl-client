#pragma once
#include "Tool_PartObject.h"

NS_BEGIN(Engine)

NS_END


NS_BEGIN(Tool)

class CTool_Weapon :
    public Tool_PartObject
{
public:
    using Super = Tool_PartObject;
    enum class Weapon_Type
    {
        SWORD, GUN, SKILL
    };

    enum class Weapon_ModelType
    {
        STATIC, ANIM
    };

    enum class State : _uint
    {
        NONE,
        HOLD,
        HAND
    };

    typedef struct tagWeaponDesc : public Tool_PartObject::PARTOBJ_DESC
    {
        std::wstring  wstrModelPrototypeName = { L"" };
        const Matrix* pMatHandSocket = { nullptr };
        const Matrix* pMatSocket = { nullptr };

        Weapon_ModelType	eModel = { Weapon_ModelType::STATIC };
        _bool				bMianWeapon = { false };
    }WEAPON_DESC;

protected:
    CTool_Weapon(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CTool_Weapon(const CTool_Weapon& rhs);
    virtual ~CTool_Weapon() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update_Priority(const _float fDT) override;
    virtual void Update(const _float fTimeDelta) override;
    virtual void Update_Late(const _float fTimeDelta) override;
    virtual void Ready_Before_Render(const _float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void Draw_ImGui() override;

public:
    //  ==========  초기 Component 설정  ================
    HRESULT Ready_Component(WEAPON_DESC* pArg);
    HRESULT Ready_Component_Texture();
    HRESULT Ready_Component_Model(void* pArg);
    HRESULT Ready_Component_Buffer(void* pArg);
    HRESULT Ready_ComputeShaders();

private:
    HRESULT Render_StaticWeap();
    HRESULT Render_AnimWeap();

public:
    static CTool_Weapon* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

protected:
    State				m_eState = { State::NONE };
    Weapon_Type			m_eWaeponType = { Weapon_Type::SWORD };
    Weapon_ModelType	m_eModleType = { Weapon_ModelType::STATIC };

    const Matrix*       m_pMatHandSocket = { nullptr };
    const Matrix*       m_pMatSocket = { nullptr };

    _bool				m_bMainWeapon = { false };
};

NS_END