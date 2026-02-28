#pragma once
#include "SkillObjectSpawnerBase.h"

NS_BEGIN(Engine)

class CSingleSkillSpawner final : public CSkillObjectSpawnerBase
{
    using Super = CSkillObjectSpawnerBase;
private:
    CSingleSkillSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    CSingleSkillSpawner(const CSingleSkillSpawner& rhs);
    virtual ~CSingleSkillSpawner() = default;

    virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc) override;
    virtual HRESULT Initialize(void* pArg) override;
protected:
    virtual _uint Get_TotalCount() const override { return 1; }
    virtual void  Emit_One(_uint, const Vec3& vForward, const Vec3&) override
    {
        Spawn_SkillObject(m_desc.vOrigin, vForward);
    }
public:
    static CSingleSkillSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
