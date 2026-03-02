#pragma once
#include "SkillObjectSpawnerBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CProjectileSpawner_Fan final : public CSkillObjectSpawnerBase
{
    using Super = CSkillObjectSpawnerBase;
public:
    typedef struct tagProjectileSpawnerFanDesc : Super::SPAWNER_COPY_DESC
    {
        _uint  iCount{ 8 };
        _float fSpreadDeg{ 45.f };
    }PR_SPAWNER_FAN_DESC;
private:
    CProjectileSpawner_Fan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    CProjectileSpawner_Fan(const CProjectileSpawner_Fan& rhs);
    virtual ~CProjectileSpawner_Fan() = default;

    virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC *pArg) override;
    virtual HRESULT Initialize(void* pArg) override;
protected:
    virtual _uint Get_TotalCount() const override { return m_iCount; }
    virtual void  Emit_One(_uint i, const Vec3& vFoward, const Vec3& vUp) override;
private:
    _uint  m_iCount{ 8 };
    _float m_fSpreadDeg{ 45.f };
public:
    static CProjectileSpawner_Fan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END