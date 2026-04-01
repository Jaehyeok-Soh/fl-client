#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

#define DASH_COUNT 4

class CMonsterActionState;
class CMonsterControlContext;

class CState_GimmikRunLoop final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	enum class ERunPhase
	{
		Dash = 0,
		Hold
	};
	typedef struct tagDashLine
	{
		Vec3 vStart{ Vec3::Zero };
		Vec3 vEnd{ Vec3::Zero };
	}DASH_LINE;
private:
	CState_GimmikRunLoop(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CState_GimmikRunLoop() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
private:
	Vec3 NormalizeXZ(Vec3 v);
	_float LengthXZ(const Vec3& v);
	_float DistanceXZ(const Vec3& vA, const Vec3& vB);
	_float AngleXZ(const Vec3& vFrom, const Vec3& vTo);
	Vec3 PointOnCircleXZ(const Vec3& vCenter, _float fRadius, _float fAngle);
	void Resolve_DashDirection();
	void Spawn_Line(const Vec3& vStart, const Vec3& vEnd);
private:
	_bool Build_DashLines(_uint iIndex);
	Vec3 Sample_PlayerPosition();
	_bool Build_DashLines();

	void Play_RunSound(const _float fTimeDelta);
private:
	CMonsterActionState* m_pOwnerActionState{ nullptr };
	CMonsterControlContext* m_pOwnerControlContext{ nullptr };
	Vec3 m_vDashDir = {};
	// TODO : BattleField
	Vec3	m_vFieldCenter = {};
	_float m_fFieldRadius = 15.f;

	const _float m_fOuterRadiusMin = 0.95f;
	const _float m_fOuterRadiusMax = 1.00f;
	const _float m_fOppositeJitterDeg = 28.f;

	_float m_fHoldTime = 0.35f;
	_float m_fHoldAcc = 0.f;

	DASH_LINE m_arrDashLine[DASH_COUNT] = {};
	_int m_iDashIndex = 0;
	ERunPhase m_ePhase = ERunPhase::Dash;

	_float m_fDashMovePerSec = 10.f;
	_float m_fArriveTolerance = 0.2f;

	_bool m_bPathReady = { false };
private:
	TimeCount	m_TSoundTimer = { 0.f,1.5f };
	const _uint m_iSoundHash_Run = TO_HASH("sfx_boss_Lianhuo_skill08_cast07");

public:
	static CState_GimmikRunLoop* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END