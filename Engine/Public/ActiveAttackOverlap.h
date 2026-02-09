#pragma once
#include "Base.h"

#include "PhysicsAttackOverlap.h"

NS_BEGIN(Engine)

class CGameInstance;
class CGameObject;

class CActiveAttackOverlap final : public CBase
{
public:
	enum Enum
	{
		WAIT,
		FIN,
	};

	using Super = CBase;
private:
	CActiveAttackOverlap();
	virtual ~CActiveAttackOverlap() = default;

	HRESULT Initialize();

public:
	void Update(_float fTimeDelta);

	void Reset();
	void Set(CPhysicsAttackOverlap::HITBOX_DESC* pDesc, Matrix ownerMatrix, CGameObject* pOwner);

	Enum GetState() { return m_eState; }

	void Tick(_float fTimeDelta);

	_bool CheckAlreadyHit(CGameObject* hitObject);

#ifdef _DEBUG
public:
	void Render();
#endif

private:
	CGameInstance* m_pGameInstance = { nullptr };

	CGameObject* m_pOwner = { nullptr };

	Matrix m_matTransform = Matrix::Identity;
	PxTransform m_pxTransform = {};
	_float m_fSumTime = {};
	_float m_fSumTickTime = {};
	CPhysicsAttackOverlap::HITBOX_DESC* m_tHitboxDesc = { nullptr };
	
	Enum m_eState = { Enum::WAIT };

	vector<PxOverlapHit> hitResults;
	PxOverlapBuffer hitBuffer;
	std::set<CGameObject*> m_hitObjects;

public:
	static CActiveAttackOverlap* Create();
	virtual void Free() override;
};

NS_END