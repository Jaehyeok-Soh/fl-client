#include "Transform.h"
#include "Engine_Utils.h"
#include "Navigation.h"
#include "Shader.h"
#include "GameObject.h"

CTransform::CTransform()
	: Super()
{
	::XMStoreFloat4x4(&m_matWorld, ::XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: Super(rhs)
{
	::XMStoreFloat4x4(&m_matWorld, ::XMMatrixIdentity());
}

HRESULT CTransform::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		COMPONENT_DESC* pDesc = static_cast<COMPONENT_DESC*>(pArg);
		if (pDesc->pTransform_Desc)
		{
			TRANSFORM_DESC* pFinalDesc = static_cast<TRANSFORM_DESC*>(pDesc->pTransform_Desc);
			Set_Info(TRANSFORM_INFO_STATE::POS, _float4(pFinalDesc->vPosition.x, pFinalDesc->vPosition.y, pFinalDesc->vPosition.z, 1.f));
			m_fMovePerSec = pFinalDesc->fMovePerSec;
			m_fRotatePerSec = pFinalDesc->fRotatePerSec;
		}
	}
	else
		Set_Info(TRANSFORM_INFO_STATE::POS, _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader)
{
	if (!pShader)
		return E_FAIL;
	return pShader->Bind_TransformData(Get_WorldMatrix());
}

_float3 ToEulerAngles(const _float4& Q)
{
	_float3 vReturnAngles = {};

	// Roll_X
	const double dSinRoll_cosp = 2 * ((Q.w * Q.x) + (Q.y * Q.z));
	const double dCosRoll_cosp = 1 - 2 * ((Q.x * Q.x) + (Q.y * Q.y));
	vReturnAngles.x = (_float)atan2(dSinRoll_cosp, dCosRoll_cosp);
	
	// Pitch_Y
	const double dSinPitch = sqrt(1 + 2 * ((Q.w * Q.y) - (Q.x * Q.z)));
	const double dCosPitch = sqrt(1 - 2 * ((Q.w * Q.y) - (Q.x * Q.z)));
	vReturnAngles.y = 2 * (_float)atan2(dSinPitch, dCosPitch) - g_XMPi.f[0] / 2;
	if (vReturnAngles.y <= g_XMEpsilon.f[0])
		vReturnAngles.y = 0.f;

	// Yaw_Z
	const double dSinYaw_cosp = 2 * ((Q.w * Q.z) + (Q.x * Q.y));
	const double dCosYaw_cosp = 1 - 2 * ((Q.y * Q.y) + (Q.z * Q.z));
	vReturnAngles.z = (_float)atan2(dSinYaw_cosp, dCosYaw_cosp);

	return vReturnAngles;
}

inline _vector CTransform::Get_Info(TRANSFORM_INFO_STATE eState)
{
	return ::XMLoadFloat4x4(&m_matWorld).r[ENUM_TO_UINT(eState)];
}

inline void CTransform::Set_Info(TRANSFORM_INFO_STATE eState, _fvector vValue)
{
	::XMStoreFloat4(reinterpret_cast<_float4*>(&m_matWorld.m[ENUM_TO_UINT(eState)]), vValue);
}

inline void CTransform::Set_Info(TRANSFORM_INFO_STATE eState, _float4 vValue)
{
	::memcpy(&m_matWorld.m[ENUM_TO_UINT(eState)], &vValue, sizeof(_float4));
}

inline _float3 CTransform::Get_Scaled()
{
	return _float3
	{
		::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::RIGHT))),
		::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::UP))),
		::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::LOOK)))
	};
}

inline _float CTransform::Get_Scaled_X()
{
	return ::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::RIGHT)));
}

inline _float CTransform::Get_Scaled_Y()
{
	return ::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::UP)));
}

inline _float CTransform::Get_Scaled_Z()
{
	return ::XMVectorGetX(::XMVector3Length(Get_Info(TRANSFORM_INFO_STATE::LOOK)));
}

inline void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::RIGHT)) * fX);
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::UP)) * fY);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::LOOK)) * fZ);
}

inline void CTransform::Set_Scale(const _float3& vValue)
{
	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::RIGHT)) * vValue.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::UP)) * vValue.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::LOOK)) * vValue.z);
}

inline void CTransform::Add_Scale(_float fX, _float fY, _float fZ)
{
	_float3 vScaled = Get_Scaled();
	vScaled.x += fX;
	vScaled.y += fY;
	vScaled.z += fZ;
	Set_Scale(vScaled);
}

inline void CTransform::Go_Dir(const _float fTimeDelta, _fvector vTargetDir, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(vTargetDir);

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;

	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Straight(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::LOOK));

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_BackWard(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::LOOK));

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Up(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::UP));

	vPosition += vDir * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Down(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::UP));

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Up(_fvector vAxis, const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);

	vPosition += vAxis * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Down(_fvector vAxis, const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);

	vPosition -= vAxis * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Right(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::RIGHT));

	vPosition += vDir * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Go_Left(const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vDir = ::XMVector3Normalize(Get_Info(TRANSFORM_INFO_STATE::RIGHT));

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale *fTimeDelta;
	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

inline void CTransform::Pitch_Turn(const _float fTimeDelta)
{
	_vector vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	_vector vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	_vector vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);

	_matrix matRotation = ::XMMatrixRotationAxis(::XMVector3Normalize(vRight), m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Yaw_Turn(const _float fTimeDelta)
{
	_vector vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	_vector vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	_vector vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);

	_matrix matRotation = ::XMMatrixRotationAxis(::XMVector3Normalize(vUp), m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Roll_Turn(const _float fTimeDelta)
{
	_vector vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	_vector vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	_vector vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);

	_matrix matRotation = ::XMMatrixRotationAxis(::XMVector3Normalize(vLook), m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 vScale = Get_Scaled();

	_vector vRight = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix matRotation = ::XMMatrixRotationAxis(vAxis, fRadian);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Rotation(_float3 vAxis, _float fRadian)
{
	_float3 vScale = Get_Scaled();
	_vector vLoadedAxis = ::XMLoadFloat3(&vAxis);

	_vector vRight = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix matRotation = ::XMMatrixRotationAxis(vLoadedAxis, fRadian);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_float3 vScale = Get_Scaled();

	_vector vRight = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_vector vQuaternion = ::XMQuaternionRotationRollPitchYaw(fX, fY, fZ);
	_matrix matRotation = ::XMMatrixRotationQuaternion(vQuaternion);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Turn_WorldYAxis(_fvector vTargetDir, const _float fTimeDelta)
{
	_vector vActorLook = ::XMVector3Normalize(::XMVectorSetY(Get_Info(TRANSFORM_INFO_STATE::LOOK), 0.f));
	_vector vTarget = ::XMVector3Normalize(::XMVectorSetY(vTargetDir, 0.f));

	_float fTarget_X = ::XMVectorGetX(vTarget);
	_float fTarget_Z = ::XMVectorGetZ(vTarget);
	_float fActorLook_X = ::XMVectorGetX(vActorLook);
	_float fActorLook_Z = ::XMVectorGetZ(vActorLook);

	// cos
	_float fResultDot = fTarget_X * fActorLook_X + fTarget_Z * fActorLook_Z;
	// sin
	_float fResultCross = fTarget_X * fActorLook_Z - fActorLook_X * fTarget_Z;
	fResultDot = std::clamp(fResultDot, -1.0f, 1.0f);
	_float fRadian = std::atan2(fResultCross, fResultDot);

	Turn(::XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian * fTimeDelta);
}

inline void CTransform::Turn(_fvector vAxis, const _float fTimeDelta)
{
	_vector vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	_vector vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	_vector vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);

	_matrix matRotation = ::XMMatrixRotationAxis(vAxis, m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3TransformNormal(vLook, matRotation));
}

inline void CTransform::Look_At(_fvector vPoint)
{
	_float3 vScale = Get_Scaled();

	_vector vLookDir = vPoint - Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vRightDir = ::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLookDir);
	_vector vUpDir = ::XMVector3Cross(vLookDir, vRightDir);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3Normalize(vRightDir) * vScale.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3Normalize(vUpDir) * vScale.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3Normalize(vLookDir) * vScale.z);
}

inline void CTransform::Look_At_XZ(_fvector vPoint)
{
	_float3 vScale = Get_Scaled();
	_vector vCurrentPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vLookDir =  ::XMVector3Normalize(::XMVectorSetY(vPoint, ::XMVectorGetY(vCurrentPosition)) - vCurrentPosition);
	_vector vRightDir = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLookDir));
	_vector vUpDir = ::XMVector3Cross(vLookDir, vRightDir);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3Normalize(vRightDir) * vScale.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3Normalize(vUpDir) * vScale.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3Normalize(vLookDir) * vScale.z);
}

inline void CTransform::Chase(_fvector vPoint, _float fMinDistance, const _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vTargetDir = vPoint - vPosition;
	_float fLength = ::XMVectorGetX(::XMVector3Length(vTargetDir));

	if (fLength >= fMinDistance)
		vPosition += ::XMVector3Normalize(vTargetDir) * m_fMovePerSec * m_fMoveScale *fTimeDelta;

	if (pNavigation == nullptr ||
		pNavigation->Is_Move(vPosition))
	{
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
	}
}

void CTransform::Start_Force(_fvector vTargetDir, _float fForceAbs, _float fDragK)
{
	if (fForceAbs <= 0.f || fDragK <= 0.f)
		return;

	_vector vDir = ::XMVector3Normalize(vTargetDir) * fForceAbs;
	::XMStoreFloat3(&m_vForceVelocity, vDir);
	m_fDragK = fDragK;
}

void CTransform::Apply_Force(_float fDeltaTime, CNavigation* pNavigation)
{
	if (m_fDragK <= 0.f)
		return;

	_vector vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vVelocity = ::XMLoadFloat3(&m_vForceVelocity);
	_vector vNext = vPosition + vVelocity * fDeltaTime;

	if (pNavigation == nullptr || pNavigation->Is_Move(vNext))
		Set_Info(TRANSFORM_INFO_STATE::POS, vNext);
	
	_float fDecay = std::exp(-m_fDragK * fDeltaTime);
	vVelocity *= fDecay;

	_float fSpeedSq = ::XMVectorGetX(::XMVector3LengthSq(vVelocity));
	if (fSpeedSq < 0.05f * 0.05f)
		Force_Clear();
	else
		::XMStoreFloat3(&m_vForceVelocity, vVelocity);
}

void CTransform::Update_PrevPosition()
{
	::memcpy(&m_vPrevPosition, m_matWorld.m[3], sizeof(_float4));
}

void CTransform::Force_Clear()
{
	m_vForceVelocity = { 0.f, 0.f, 0.f };
	m_fDragK = { 0.f };
}

CTransform* CTransform::Create()
{
	CTransform* pInstance = new CTransform();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CTransform::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pClone = new CTransform(*this);
	
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CTransform::Clone, Failed");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTransform::Free()
{
	Super::Free();
}