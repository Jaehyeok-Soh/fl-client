#include "pch.h"
#include "Gun.h"

CGun::CGun(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext, Weapon_Type::GUN)
{
}

CGun::CGun(const CGun& rhs)
	:Super(rhs)
	, m_fSpeed(rhs.m_fSpeed)
{
}

HRESULT CGun::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGun::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	GUN_DESC* pDesc = static_cast<GUN_DESC*>(pArg);

	m_MTotalBullet = { pDesc->fAllBullet,pDesc->fAllBullet };
	m_MCurBullet = { pDesc->fCurBullet ,pDesc->fCurBullet };

	m_tFireTimeCounter.bCountTime	= false;
	m_tFireTimeCounter.bTimeReset	= true;
	m_tFireTimeCounter.fMaxTime		= pDesc->fAttackCoolTime;
	m_tFireTimeCounter.fTimeAcc = pDesc->fAttackCoolTime; // 처음에 바로 쏠 수 있도록 하기 위함

	return S_OK;
}

HRESULT CGun::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CGun::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGun::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Attack_Update(fTimeDelta);
}

void CGun::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CGun::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CGun::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CGun::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CGun::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CGun::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);
}

void CGun::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CGun::Render()
{
	return Super::Render();
}

void CGun::Reload_Bullet()
{
	// 한번에 재장전 할 양이 충분하다면
	if (m_MTotalBullet.x >= m_MCurBullet.y)
	{
		// Total 값 빼주고
		m_MTotalBullet.x -= m_MCurBullet.y;

		// Cur 값 더해줌
		m_MCurBullet.x = m_MCurBullet.y;
	}

	// 충분하지 않다면
	else
	{
		// Cur값 바로 total.x 값으로 셋팅
		m_MCurBullet.x = m_MTotalBullet.x;

		// total.x == 0
		m_MTotalBullet.x = 0.f;
	}
}

_bool CGun::Get_CanFire()
{
	return (m_MCurBullet.x > 0.f);
}

_bool CGun::Get_CanReleod()
{
	return (m_MTotalBullet.x > 0.f);
}

void CGun::NoAttack_Update(const _float fTimeDelta)
{
}

void CGun::Attack_Update(const _float fTimeDelta)
{
	// 총을 쏠 수 있을때
	if (m_tFireTimeCounter.bCountTime)
	{
		// Cool Timer가 다 되었다면 
		if (m_tFireTimeCounter.CountTime(fTimeDelta) == 1.f)
		{
			// 실제로 총격 판정
			Fire();
		}
	}

	//// 총을 쏠 수 없는데 reload도 못하는 상태 -> no att
	//else if (Get_CanReleod())
	//	Change_GunState(GunState::NOATT);

	//// reload도 가능 하다면 일단 empty
	//else
	//	Change_GunState(GunState::EMPTY);
}

void CGun::Empty_Update(const _float fTimeDelta)
{
}

void CGun::Reload_Update(const _float fTimeDelta)
{
}

void CGun::Fire()
{
	m_MCurBullet.x -=1.f;

	if (m_MCurBullet.x == 0.f)
		Reset_FireTimer();
}

CGun* CGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGun* pInsatnce = new CGun(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CGun::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CGun::Clone(void* pArg)
{
	CGun* pClone = new CGun(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CGun::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CGun::Free()
{
	__super::Free();
}