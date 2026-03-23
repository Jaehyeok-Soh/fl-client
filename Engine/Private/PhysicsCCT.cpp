#include "Engine_pch.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "Physics_CCTFilterCallback.h"

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
	, m_fGravityOffset(rhs.m_fGravityOffset)
	, m_bYLerp(rhs.m_bYLerp)
	, m_fSpeedOffset(rhs.m_fSpeedOffset)
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

	m_tMoveState.bGravity = m_tDesc.bGravity;
	m_tMoveState.fGravity = m_tDesc.fGravity;
	m_tMoveState.CMSpeed.y = m_tDesc.MSpeed.x;
	m_tMoveState.CMSpeed.z = m_tDesc.MSpeed.y;
	m_tMoveState.CMAccelRate.x = m_tDesc.MAccelRate.y;
	m_tMoveState.CMAccelRate.y = m_tDesc.MAccelRate.x;
	m_tMoveState.CMAccelRate.z = m_tDesc.MAccelRate.y;
	m_tMoveState.CMDeAccelRate.x = m_tDesc.MDeAccelRate.y;
	m_tMoveState.CMDeAccelRate.y = m_tDesc.MDeAccelRate.x;
	m_tMoveState.CMDeAccelRate.z = m_tDesc.MDeAccelRate.y;

	m_fHeightOffset = m_tDesc.fHeight * 0.5f;
	Set_Owner(m_tDesc.pOwner);

	GetController();
	SetUserData(static_cast<void*>(m_tDesc.pOwner));

	auto initPos = m_pController->getFootPosition();
	m_vPoolingPosition = Vec3(static_cast<_float>(initPos.x), static_cast<_float>(initPos.y), static_cast<_float>(initPos.z));

	SetCollisionFilter();

	m_pCCTFilterCallback = m_pGameInstance->GetCCTFilterCallback();
	m_pQueryFilterCallback = m_pGameInstance->GetQueryFilterCallback();
	m_pQueryFilterCallback->SetOwner(Get_Owner());

	m_fContactOffset = m_pController->getContactOffset();

	auto cctActor = m_pController->getActor();
	PxShape* cctShape = { nullptr };
	if (cctActor->getNbShapes() > 0)
		cctActor->getShapes(&cctShape, 1);

	if (cctShape)
	{
		PxFilterData filterData;
		filterData.word0 = m_tDesc.eFilterLayer;
		filterData.word1 = m_tDesc.iFilterMask;
		cctShape->setSimulationFilterData(filterData);
		cctShape->setQueryFilterData(filterData);
		cctShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		cctShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	}

	EnableCollision(false);

	return S_OK;
}

void CPhysicsCCT::Ready_Position()
{
	Vec3 vPos = m_pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	SetFootPosition(vPos);

	EnableCollision(true);
}

void CPhysicsCCT::Update(const _float fTimeDelta)
{
	UpdateMove(fTimeDelta);
}

#ifdef _DEBUG
void CPhysicsCCT::Render()
{
	m_pGameInstance->Physics_Render(m_pController->getActor());
}
#endif // _DEBUG

void CPhysicsCCT::UpdateMove(const _float fTimeDelta)
{
	_bool bHasInput = m_tMoveState.vInputDir.magnitudeSquared() > 0.001f;

	m_tMoveState.vTargetVelocity = m_tMoveState.vInputDir * m_tMoveState.CMSpeed.z * m_fSpeedOffset;
	m_tMoveState.vTargetVelocity.y = 0.f;

	PxVec3 currentHorizontalVel = m_tMoveState.vVelocity;
	currentHorizontalVel.y = 0.f;

	PxVec3 velocityDiff = m_tMoveState.vTargetVelocity - currentHorizontalVel;

	_float curRate = bHasInput ? m_tMoveState.CMAccelRate.x : m_tMoveState.CMDeAccelRate.x;

	m_tMoveState.vAccelation = velocityDiff * curRate;

	ApplyImpuls(fTimeDelta);
	ApplyExternAcc(fTimeDelta);
	ApplyGravity(fTimeDelta);

	m_tMoveState.vVelocity += m_tMoveState.vAccelation * fTimeDelta;

	m_tMoveState.vVelocity.y = PxClamp(
		m_tMoveState.vVelocity.y,
		m_tMoveState.CMVerticalSpeed.y,
		m_tMoveState.CMVerticalSpeed.z
	);

	if (m_tDesc.bIsHover && m_bEnableMove == true)
	{
		Vec3 finalPos = GetFootPosition();

		_float fHitDesc = {};
		Vec3 vHitPos = {};
		Vec3 vRayPos = finalPos;
		vRayPos.y += 0.3f;

		if (m_pGameInstance->RayCast(vRayPos, Vec3(0.f, -1.f, 0.f), 500.f, m_pQueryFilterCallback, &fHitDesc, &vHitPos))
		{
			_float fMinHoverY = vHitPos.y + m_tDesc.fHoverOffset;

			Vec3 vHoverPos(0.f, vHitPos.y + m_tDesc.fHoverOffset, 0.f);
			Vec3 vCurrentPose(0.f, finalPos.y, 0.f);

			vHoverPos = vHoverPos - vCurrentPose;

			vHoverPos *= 5.f * fTimeDelta;

			AddFixedMove(vHoverPos);
		}
	}

	m_CollisionFlags = Move((m_tMoveState.vVelocity * fTimeDelta) + m_tMoveState.vFixedMove, 0.001f, fTimeDelta);

	m_tMoveState.ReadyNext();
}

void CPhysicsCCT::SetInputDir(Vec3 vInputDir)
{
	m_tMoveState.vInputDir = ToPxVec3(vInputDir);
}

void CPhysicsCCT::AddAccelation(Vec3 vAccelation)
{
	m_tMoveState.vExternAccelation += ToPxVec3(vAccelation);
}

void CPhysicsCCT::AddFixedMove(Vec3 vVelocity)
{
	m_tMoveState.vFixedMove += ToPxVec3(vVelocity);
}

void CPhysicsCCT::SetImpulsAccelation(Vec3 vImpuls)
{
	m_tMoveState.vImpulsAccelation += ToPxVec3(vImpuls);
}

void CPhysicsCCT::SetZeroVelocity()
{
	m_tMoveState.vVelocity = { 0.f, 0.f, 0.f };
}

void CPhysicsCCT::SetZeroHorizontalVelocity()
{
	m_tMoveState.vVelocity.x = 0.f;
	m_tMoveState.vVelocity.z = 0.f;
}

void CPhysicsCCT::SetZeroVerticalVelocity()
{
	m_tMoveState.vVelocity.y = 0.f;
}

void CPhysicsCCT::SetZeroDeAccelRate()
{
	m_tMoveState.CMDeAccelRate.x = m_tMoveState.CMDeAccelRate.y;
}

void CPhysicsCCT::SetDeAccelRate(_float fRate)
{
	m_tMoveState.CMDeAccelRate.x = fRate;
}

void CPhysicsCCT::ResetDeAccelRate()
{
	m_tMoveState.CMDeAccelRate.x = m_tMoveState.CMDeAccelRate.z;
}

void CPhysicsCCT::ApplyGravity(const _float fTimeDelta)
{
	if (m_tMoveState.bGravity == false)
		return;

	if (m_CollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
		m_tMoveState.vAccelation.y += (-0.1f + m_fGravityOffset);
	else
		m_tMoveState.vAccelation.y += (m_tMoveState.fGravity + m_fGravityOffset);
}

void CPhysicsCCT::ApplyExternAcc(const _float fTimeDelta)
{
	m_tMoveState.vAccelation += m_tMoveState.vExternAccelation;
}

void CPhysicsCCT::ApplyImpuls(const _float fTimeDelta)
{
	m_tMoveState.vVelocity += m_tMoveState.vImpulsAccelation;
}

void CPhysicsCCT::SetHeight(_float height)
{
	switch (m_tDesc.eType)
	{
	case EPhysicsCCTType::BOX:
	{
		static_cast<PxBoxController*>(m_pController)->setHalfHeight(height * 0.5f);
		//m_pController->resize(height * 0.5f);
	}
	break;
	case EPhysicsCCTType::CAPSULE:
	{
		static_cast<PxCapsuleController*>(m_pController)->setHeight(height);
		//m_pController->resize(height * 0.5f);
	}
	break;
	}
}

const PxControllerCollisionFlags CPhysicsCCT::Move(PxVec3 disp, _float minDist, _float fTimeDelta)
{
	_bool bDisableMove = m_pGameInstance->Is_ChangeLevelSequence() || m_bEnableCollision == false;

	PxFilterData queryFilterData;
	queryFilterData.word0 = m_tDesc.eFilterLayer;
	queryFilterData.word1 = ~(PHYSICSFILTERGROUP::RAGDOLL);

	PxControllerFilters filters;
	filters.mCCTFilterCallback = (PxControllerFilterCallback*)m_pCCTFilterCallback;
	filters.mFilterCallback = m_pQueryFilterCallback;

	PxControllerCollisionFlags collisionFlag;
	if (m_bEnableMove && bDisableMove == false)
		collisionFlag = m_pController->move(disp, minDist, fTimeDelta, filters);

	if (m_bIsSteppingOnCCT)
		collisionFlag &= ~PxControllerCollisionFlag::eCOLLISION_DOWN;

	if (m_bIsSideOnCCT)
		collisionFlag &= ~PxControllerCollisionFlag::eCOLLISION_SIDES;

	m_bIsSteppingOnCCT = false;
	m_bIsSideOnCCT = false;

	Vec3 finalPos = GetFootPosition();
	CTransform* transform = m_pOwner->Get_Component<CTransform>();

	if (!m_tDesc.vLocalOffset.InBounds(Vec3(1e-5f, 1e-5f, 1e-5f)))
	{
		Vec3 vRight = transform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
		Vec3 vUp = transform->Get_Info(TRANSFORM_INFO_STATE::UP);
		Vec3 vLook = transform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

		vRight.Normalize();
		vUp.Normalize();
		vLook.Normalize();

		finalPos += vRight * m_tDesc.vLocalOffset.x;
		finalPos += vUp * m_tDesc.vLocalOffset.y;
		finalPos += vLook * m_tDesc.vLocalOffset.z;
	}

	if (!m_tDesc.vLocalOffset.InBounds(Vec3(1e-5f, 1e-5f, 1e-5f)))
	{
		finalPos.x += m_tDesc.vWorldOffset.x;
		finalPos.y += m_tDesc.vWorldOffset.y;
		finalPos.z += m_tDesc.vWorldOffset.z;
	}

	if (m_bYLerp)
	{
		Vec3 currentPos = transform->Get_Info(TRANSFORM_INFO_STATE::POS);

		_float fLerpAmount = fTimeDelta * 15.f;
		if (fLerpAmount > 1.f)
			fLerpAmount = 1.f;

		_float yLerp = std::lerp(currentPos.y, finalPos.y, fLerpAmount);
		finalPos.y = yLerp;
	}

	// test
	else
	{
		int  a = 0;
	}

	transform->Set_Info(TRANSFORM_INFO_STATE::POS, finalPos);

	if (collisionFlag & PxControllerCollisionFlag::eCOLLISION_DOWN)
		m_tMoveState.vVelocity.y = 0;

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
	m_pController->getActor()->userData = data;
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
	if (m_pController)
	{
		m_pController->setUserData(nullptr);
		m_pController->getActor()->userData = nullptr;
		PX_RELEASE(m_pController);
	}
}

void CPhysicsCCT::SetCollisionFilter()
{
	_uint numShape = m_pController->getActor()->getNbShapes();
	vector<PxShape*> shapes(numShape);
	m_pController->getActor()->getShapes(shapes.data(), numShape);

	PxFilterData filter(m_tDesc.eFilterLayer, m_tDesc.iFilterMask, 0, 0);

	for (auto& shape : shapes)
	{
		shape->setSimulationFilterData(filter);
		shape->setQueryFilterData(filter);
	}
}

void CPhysicsCCT::SetCollisionFilter_Empty()
{
	_uint numShape = m_pController->getActor()->getNbShapes();
	vector<PxShape*> shapes(numShape);
	m_pController->getActor()->getShapes(shapes.data(), numShape);

	PxFilterData filter(0, 0, 0, 0);

	for (auto& shape : shapes)
	{
		shape->setSimulationFilterData(filter);
		shape->setQueryFilterData(filter);
	}
}

void CPhysicsCCT::SetIsSteppingOnCCT()
{
	m_bIsSteppingOnCCT = true;
}

void CPhysicsCCT::SetIsSideOnCCT()
{
	m_bIsSideOnCCT = true;
}

void CPhysicsCCT::EnableCollision(_bool bEnable)
{
	if (m_pController == nullptr)
		return;

	m_bEnableCollision = bEnable;

	PxRigidDynamic* pActor = m_pController->getActor();
	if (pActor)
	{
		PxU32 numShape = pActor->getNbShapes();
		vector<PxShape*> vecShape(numShape);
		pActor->getShapes(vecShape.data(), numShape);

		if (vecShape.size() > 0)
		{
			for (auto& shape : vecShape)
			{
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, bEnable);
				shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bEnable);
			}
		}
	}

	if (bEnable)
		SetCollisionFilter();
	else
		SetCollisionFilter_Empty();
}

void CPhysicsCCT::SetPoolingPosition(Vec3 vPos)
{
	m_vPoolingPosition = vPos;
}

void CPhysicsCCT::ApplyPoolingPosition()
{
	SetFootPosition(m_vPoolingPosition);
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
	Safe_Release(m_pQueryFilterCallback);

	ReleaseController();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Super::Free();
}
