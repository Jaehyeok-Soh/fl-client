#include "Engine_pch.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"
#include "GameObject.h"

CPhysicsCCT::CPhysicsCCT(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CPhysicsCCT::CPhysicsCCT(const CPhysicsCCT& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CPhysicsCCT::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysicsCCT::Initialize(void* pArg)
{
	m_tDesc = *static_cast<PHYSICSCCT_DESC*>(pArg);
	m_fHeightOffset = m_tDesc.fHeight * 0.5f;
	Set_Owner(m_tDesc.pOwner);

	GetController();
	SetUserData(static_cast<void*>(m_tDesc.pOwner));

	m_fContactOffset = m_pController->getContactOffset();

	return S_OK;
}

void CPhysicsCCT::Awake()
{
	//Vec3 vPos = m_pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	//SetPosition(vPos);
}

void CPhysicsCCT::Update()
{
}

#ifdef _DEBUG
void CPhysicsCCT::Render()
{
	m_pGameInstance->Physics_Render(m_pController->getActor());
}
#endif // _DEBUG

void CPhysicsCCT::SetHeight(_float height)
{
	switch (m_tDesc.eType)
	{
	case EPhysicsCCTType::BOX:
	{
		static_cast<PxBoxController*>(m_pController)->setHalfHeight(height * 0.5f);
		m_pController->resize(height * 0.5f);
	}
	break;
	case EPhysicsCCTType::CAPSULE:
	{
		static_cast<PxCapsuleController*>(m_pController)->setHeight(height);
		m_pController->resize(height * 0.5f);
	}
	break;
	}
}

const PxControllerCollisionFlags CPhysicsCCT::Move(Vec3 disp, _float minDist, _float fTimeDelta)
{
	PxVec3 displacementVec(disp.x, disp.y, disp.z);
	PxControllerFilters filters;
	const PxControllerCollisionFlags collisionFlag = m_pController->move(displacementVec, minDist, fTimeDelta, filters);

	return collisionFlag;
}

void CPhysicsCCT::SetPosition(Vec3 position)
{
	m_pController->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Vec3 CPhysicsCCT::GetPosition()
{
	PxExtendedVec3 position = m_pController->getPosition();
	return Vec3((_float)position.x, (_float)position.y, (_float)position.z);
}

void CPhysicsCCT::SetFootPosition(Vec3 pos)
{
	m_pController->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

Vec3 CPhysicsCCT::GetFootPosition()
{
	PxExtendedVec3 pos = m_pController->getFootPosition();
	return Vec3((_float)pos.x, (_float)pos.y, (_float)pos.z);
}

Vec3 CPhysicsCCT::GetUpDirection()
{
	PxVec3 dir = m_pController->getUpDirection();
	return Vec3(dir.x, dir.y, dir.z);
}

void CPhysicsCCT::SetUpDirection(Vec3 dir)
{
	m_pController->setUpDirection(PxVec3(dir.x, dir.y, dir.z));
}

_float CPhysicsCCT::GetStepOffset()
{
	return m_pController->getStepOffset();
}

void CPhysicsCCT::SetStepOffset(_float offset)
{
	m_pController->setStepOffset(offset);
}

void CPhysicsCCT::SetNonWalkableMode(PxControllerNonWalkableMode::Enum mode)
{
	m_pController->setNonWalkableMode(mode);
}

PxControllerNonWalkableMode::Enum CPhysicsCCT::GetNonWalkableMode()
{
	return m_pController->getNonWalkableMode();
}

void CPhysicsCCT::SetContactOffset(_float offset)
{
	m_pController->setContactOffset(offset);
}

_float CPhysicsCCT::GetContactOffset()
{
	return m_pController->getContactOffset();
}

void CPhysicsCCT::SetSlopeLimit(_float limit)
{
	m_pController->setSlopeLimit(limit);
}

_float CPhysicsCCT::GetSlopeLimit()
{
	return m_pController->getSlopeLimit();
}

void CPhysicsCCT::CacheReset()
{
	m_pController->invalidateCache();
}

void CPhysicsCCT::SetUserData(void* data)
{
	m_pController->setUserData(data);
}

void CPhysicsCCT::GetState(PxControllerState& outState)
{
	return m_pController->getState(outState);
}

void CPhysicsCCT::GetController()
{
	m_pController = m_pGameInstance->GetController(&m_tDesc);
}

void CPhysicsCCT::ReleaseController()
{
	PX_RELEASE(m_pController);
}

CPhysicsCCT* CPhysicsCCT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPhysicsCCT* pInstance = new CPhysicsCCT(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysicsCCT");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsCCT::Clone(void* pArg)
{
	CPhysicsCCT* pInstance = new CPhysicsCCT(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsCCT");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsCCT::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Super::Free();
}
