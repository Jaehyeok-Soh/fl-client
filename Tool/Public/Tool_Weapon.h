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

    enum class Weapon_ModelType
    {
        STATIC, ANIM
    };

    enum  State : _uint
    {
        NONE,
        HOLD
    };

    typedef struct tagWeaponDesc : public Tool_PartObject::PARTOBJ_DESC
    {
        std::wstring  wstrModelPrototypeName = { L"" };
        const Matrix* pMatSocket = { nullptr };

        _int iSocketIdx = -1;

        Weapon_ModelType	eModel = { Weapon_ModelType::STATIC };
    }WEAPON_DESC;

    typedef struct tagWeaponInfo
    {
        _int iSocketIdx = -1;

        _float vScale[3] = {1.f,1.f,1.f};
        _float vPYR[3] = { 0.f,0.f,0.f };
        _float vTranslation[3] = { 0.f,0.f,0.f };

    }WEAPON_INFO;

    enum class SRT { Scale, PYR, Translation };

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
    void Set_Soket(_uint iIdx, _bool bCombine);
    void Set_State(_uint iState);
    void Set_SRT(SRT eSRT, Vec3 vValue);
    const WEAPON_INFO& Get_Info() const { return m_tWeaponInfo; }

public:
    //  ==========  초기 Component 설정  ================
    HRESULT Ready_Component(WEAPON_DESC* pArg);
    HRESULT Ready_ComputeShaders();

private:
    State				m_eState = { State::NONE };
    Weapon_ModelType	m_eModleType = { Weapon_ModelType::STATIC };

    const Matrix*       m_pMatSocket = { nullptr };

    Matrix				m_matRotation = {  };

    WEAPON_INFO         m_tWeaponInfo = {};

private:
    void	Play_Anim(const _float fTimeDelta);

    HRESULT Render_StaticWeap();
    HRESULT Render_AnimWeap();

public:
    static CTool_Weapon* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END