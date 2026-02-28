#pragma once
#include "SkillObjectSpawnerBase.h"

NS_BEGIN(Engine)

class CSkillObjectSpawner_RandomXZ final : public CSkillObjectSpawnerBase
{
	using Super = CSkillObjectSpawnerBase;

public:
	typedef struct tagSpawnerRandDesc : Super::SPAWNER_COPY_DESC
	{
		_float  fDurationSec = { 2.f };   // 몇초 동안?
		_uint  iPerTick = { 1 };    // 1텀에 몇 개?
		_float fRadiusMin = { 0.f };
		_float fRadiusMax = { 5.f };

		_bool  bUseForwardDir = { false }; // 번개처럼 정지형이면 false 권장
		Vec3   vFixedDir = { 0.f, 0.f, 0.f }; // 필요하면 고정 방향(예: 아래)
	}SPAWNER_RAND_COPY_DESC;
private:
	CSkillObjectSpawner_RandomXZ(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSkillObjectSpawner_RandomXZ(const CSkillObjectSpawner_RandomXZ& rhs);
	virtual ~CSkillObjectSpawner_RandomXZ() = default;

	virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC* pOriginDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	// 텀 횟수
	virtual _uint Get_TotalCount() const override;
	virtual void  Emit_One(_uint i, const Vec3& vForwardIn, const Vec3& vUpIn) override;

private:
	Vec3 Make_RandomPos_OnPlane(const Vec3& vOrigin, const Vec3& vForward, const Vec3& vUp) const;
private:
	SPAWNER_RAND_COPY_DESC m_desc{};
public:
	static CSkillObjectSpawner_RandomXZ* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pOriginDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END