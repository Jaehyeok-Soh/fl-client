#pragma once
#include "SkillObjectSpawnerBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CProjectileSpawner_Radial360 final : public CSkillObjectSpawnerBase
{
    using Super = CSkillObjectSpawnerBase;
public:
    typedef struct tagProjectileSpawnerRadialDesc : Super::SPAWNER_COPY_DESC
    {
        _uint  iCount{ 12 };
    }PR_SPAWNER_RADIAL_DESC;
private:
    CProjectileSpawner_Radial360(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    CProjectileSpawner_Radial360(const CProjectileSpawner_Radial360& rhs);
    virtual ~CProjectileSpawner_Radial360() = default;

    virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC* pArg) override;
    virtual HRESULT Initialize(void* pArg) override;
protected:
    virtual _uint Get_TotalCount() const override { return m_desc.iCount; }
    virtual void  Emit_One(_uint i, const Vec3& vFoward, const Vec3& vUp) override;

private:
    PR_SPAWNER_RADIAL_DESC m_desc{};
public:
    static CProjectileSpawner_Radial360* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END