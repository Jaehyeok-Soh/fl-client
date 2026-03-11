#pragma once
#include "SkillObjectSpawnerBase.h"

#define GATE_COUNT 8

NS_BEGIN(Client)

class CXibi_GateSpawner final : public CSkillObjectSpawnerBase
{
    using Super = CSkillObjectSpawnerBase;
public:
    typedef struct tagXibiGateSeqDesc : Super::SPAWNER_COPY_DESC
    {
        _float fSpreadYawDeg = 4.f;   // ÁÂ¿ì ·£´ý
        _float fSpreadPitchDeg = 2.f; // »óÇÏ ·£´ý
    }XIBIGATE_COPTY_DESC;
private:
    CXibi_GateSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CXibi_GateSpawner(const CXibi_GateSpawner& rhs);
    virtual ~CXibi_GateSpawner() = default;

    virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc) override;
    virtual HRESULT Initialize(void* pArg) override;

protected:
    virtual _uint Get_TotalCount() const override { return 13; }
    virtual void Emit_One(_uint i, const Vec3& vForward, const Vec3& vUp) override;

private:
    Vec3 Compute_GateWorldPos(_uint iGate, const Vec3& vOrigin, const Vec3& vForward, const Vec3& vUp) const;
    Vec3 Apply_RandomSpread(const Vec3& vBaseDir, const Vec3& vUp) const;

private:
    Vec3* m_pGateOffsets = { nullptr };
    _float m_fSpreadYawDeg = { 4.f };
    _float m_fSpreadPitchDeg = { 2.f };
public:
    static CXibi_GateSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SPAWNER_ORIGIN_DESC* pDesc);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END

