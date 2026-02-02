#include "Engine_pch.h"
#include "Transform.h"
#include "Engine_Utils.h"
#include "Navigation.h"
#include "Shader.h"
#include "GameObject.h"

CTransform::CTransform()
	: Super()
{
	m_matWorld = Matrix::Identity;
}

CTransform::CTransform(const CTransform& rhs)
	: Super(rhs)
{
	m_matWorld = Matrix::Identity;
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

			Matrix B =
			{
				1,  0,  0,  0,
				0,  0, -1,  0,
				0,  1,  0,  0,
				0,  0,  0,  1
			};
			Matrix InvB = B.Transpose();

			Matrix vScale	 = Matrix::CreateScale(pFinalDesc->vScale);
			Matrix vRotation = Matrix::Identity;
			if(pFinalDesc->bInstance == false)
			{
				float pitch = XMConvertToRadians(pFinalDesc->vRotation_Degrees.x) * 0.5f;
				float yaw = XMConvertToRadians(pFinalDesc->vRotation_Degrees.y) * 0.5f;
				float roll = XMConvertToRadians(pFinalDesc->vRotation_Degrees.z) * 0.5f;

				const float SP = sinf(pitch), CP = cosf(pitch);
				const float SY = sinf(yaw), CY = cosf(yaw);
				const float SR = sinf(roll), CR = cosf(roll);

				// (x,y,z,w) 순서로 반환 (SimpleMath Quaternion과 동일)
				Quat q;
				q.x = CR * SP * SY - SR * CP * CY;
				q.y = -CR * SP * CY - SR * CP * SY;
				q.z = CR * CP * SY - SR * SP * CY;
				q.w = CR * CP * CY + SR * SP * SY;

				// 비교 편의상 부호 표준화(선택): q와 -q는 같은 회전
				if (q.w < 0.f) q = Quat(-q.x, -q.y, -q.z, -q.w);

				
				vRotation = Matrix::CreateFromQuaternion(q);
			}
			else
			{
				Quat qV = pFinalDesc->vQuaternion;
				if (qV.w < 0.f) qV = Quat(-qV.x, -qV.y, -qV.z, -qV.w);

				vRotation = Matrix::CreateFromQuaternion(qV);
			}

			Matrix vPos		 = Matrix::CreateTranslation(pFinalDesc->vPosition);

			m_matWorld = vScale*  vRotation  * vPos;
			m_matWorld = InvB * m_matWorld * B;
			m_fMovePerSec = pFinalDesc->fMovePerSec;
			m_fRotatePerSec = pFinalDesc->fRotatePerSec;
		}
	}
	else
		Set_Info(TRANSFORM_INFO_STATE::POS, Vec3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader)
{
	if (!pShader)
		return E_FAIL;
	return pShader->Bind_TransformData(Get_WorldMatrix());
}

inline Matrix CTransform::Get_WorldMatrix_Transpose()
{
	return m_matWorld.Transpose();
}

inline Matrix CTransform::Get_WorldMatrix_Inverse()
{
	return m_matWorld.Invert();
}

inline Vec3 CTransform::Get_Info(TRANSFORM_INFO_STATE eState)
{
	return Vec3(m_matWorld.m[ENUM_TO_UINT(eState)][0], m_matWorld.m[ENUM_TO_UINT(eState)][1], m_matWorld.m[ENUM_TO_UINT(eState)][2]);
}

inline void CTransform::Set_Info(TRANSFORM_INFO_STATE eState, Vec3 vValue)
{
	switch (eState)
	{
	case Engine::TRANSFORM_INFO_STATE::RIGHT:
		m_matWorld.Right(vValue);
		break;
	case Engine::TRANSFORM_INFO_STATE::UP:
		m_matWorld.Up(vValue);
		break;
	case Engine::TRANSFORM_INFO_STATE::LOOK:
		m_matWorld.Backward(vValue);
		break;
	case Engine::TRANSFORM_INFO_STATE::POS:
		m_matWorld.Translation(vValue);
		break;
	}
}

inline Vec3 CTransform::Get_Scaled()
{
	return Vec3
	{
		Get_Info(TRANSFORM_INFO_STATE::RIGHT).Length(),
		Get_Info(TRANSFORM_INFO_STATE::UP).Length(),
		Get_Info(TRANSFORM_INFO_STATE::LOOK).Length()
	};
}

inline void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	vRight.Normalize();
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vLook.Normalize();
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	vUp.Normalize();

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight * fX);
	Set_Info(TRANSFORM_INFO_STATE::UP, vUp * fY);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook * fZ);
}

inline void CTransform::Set_Scale(const Vec3& vValue)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	vRight.Normalize();
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vLook.Normalize();
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	vUp.Normalize();

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight * vValue.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, vUp * vValue.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook * vValue.z);
}

inline void CTransform::Add_Scale(_float fX, _float fY, _float fZ)
{
	Vec3 vScaled = Get_Scaled();
	vScaled.x += fX;
	vScaled.y += fY;
	vScaled.z += fZ;
	Set_Scale(vScaled);
}

inline void CTransform::Go_Dir(const Vec3& vTargetDir, const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = vTargetDir;
	vDir.Normalize();

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Straight(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vDir.Normalize();

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_BackWard(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vDir.Normalize();

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Up(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::UP);
	vDir.Normalize();

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Down(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::UP);
	vDir.Normalize();

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Up(const Vec3& vAxis, const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);

	vPosition += vAxis * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Down(const Vec3& vAxis, const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);

	vPosition -= vAxis * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Right(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	vDir.Normalize();

	vPosition += vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Go_Left(const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vDir = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	vDir.Normalize();

	vPosition -= vDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;
	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

inline void CTransform::Rotation(TRANSFORM_INFO_STATE eState, _float fRadian)
{
	Vector3 vAxis = Get_Info(eState);
	vAxis.Normalize();
	Rotation(vAxis, fRadian);
}

inline void CTransform::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
	// 사원수를 통해 회전 행렬을 구함
	Quat vQuaternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
	Matrix RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	// r,u,l 의 scale 값 포함해서 표현
	Vec3 vScale = this->Get_Scaled();
	Vec3 vRight = Vec3(1.f,0.f,0.f) * vScale.x;
	Vec3 vUp = Vec3(0.f,1.f,0.f) * vScale.y;
	Vec3 vLook = Vec3(0.f,0.f,1.f) * vScale.z;

	// r,u,l 각각 *RotMat
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	// 완성한 r,u,l worldMat에 셋팅
	Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRight);
	Set_Info(TRANSFORM_INFO_STATE::UP, vUp);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, vLook);
}

inline void CTransform::Pitch_Turn(const _float fTimeDelta)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vRight.Normalize();

	Matrix matRotation = Matrix::CreateFromAxisAngle(vRight, m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}

inline void CTransform::Yaw_Turn(const _float fTimeDelta)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vUp.Normalize();

	Matrix matRotation = Matrix::CreateFromAxisAngle(vUp, m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}

inline void CTransform::Roll_Turn(const _float fTimeDelta)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vLook.Normalize();

	Matrix matRotation = Matrix::CreateFromAxisAngle(vLook, m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}

inline void CTransform::Rotation(const Vec3& vAxis, _float fRadian)
{
	Vec3 vScale = Get_Scaled();

	Vec3 vRight = Vec3::Right * vScale.x;
	Vec3 vUp = Vec3::Up * vScale.y;
	Vec3 vLook = Vec3::Backward * vScale.z;
	
	Matrix matRotation = Matrix::CreateFromAxisAngle(vAxis, fRadian);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}

inline void CTransform::Rotation(const Quat& vQuat)
{
	Vec3 vScale = Get_Scaled();

	Vec3 vRight = Vec3::Right * vScale.x;
	Vec3 vUp = Vec3::Up * vScale.y;
	Vec3 vLook = Vec3::Backward * vScale.z;

	Quat vNormalQuat = vQuat;
	vNormalQuat.Normalize();

	Matrix matRotation = Matrix::CreateFromQuaternion(vNormalQuat);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}


inline void CTransform::Turn_WorldYAxis(const Vec3& vTargetDir, const _float fTimeDelta)
{
	Vec3 vActorLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vActorLook.y = 0.0f;
	if (vActorLook.LengthSquared() < g_XMEpsilon.f[0])
		return;
	vActorLook.Normalize();

	Vec3 vTarget = vTargetDir;
	vTarget.y = 0.0f;
	if (vTarget.LengthSquared() < g_XMEpsilon.f[0])
		return;
	vTarget.Normalize();

	const _float fDot = std::clamp(vActorLook.Dot(vTarget), -1.f, 1.f);
	const _float fCross = vActorLook.Cross(vTarget).y;
	const _float fRadian = std::atan2(fCross, fDot);

	Turn(Vector3::UnitY, fRadian * fTimeDelta);
}

inline void CTransform::Turn(const Vec3& vAxis, const _float fTimeDelta)
{
	Vec3 vRight = Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vUp = Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vLook = Get_Info(TRANSFORM_INFO_STATE::LOOK);

	Matrix matRotation = Matrix::CreateFromAxisAngle(vAxis, m_fRotatePerSec * fTimeDelta);

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, Vec3::TransformNormal(vRight, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::UP, Vec3::TransformNormal(vUp, matRotation));
	Set_Info(TRANSFORM_INFO_STATE::LOOK, Vec3::TransformNormal(vLook, matRotation));
}

inline void CTransform::Look_At(const Vec3& vPoint)
{
	Vec3 vScale = Get_Scaled();
	Vec3 vLookDir = vPoint - Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vRightDir = Vec3::UnitY.Cross(vLookDir);
	Vec3 vUpDir = vLookDir.Cross(vRightDir);
	vLookDir.Normalize();
	vRightDir.Normalize();
	vUpDir.Normalize();

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRightDir * vScale.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, vUpDir * vScale.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, vLookDir * vScale.z);
}

inline void CTransform::Look_At_XZ(Vec3 vPoint)
{
	Vec3 vScale = Get_Scaled();
	Vec3 vCurrentPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	vPoint.y = vCurrentPosition.y;
	Vec3 vLookDir = vPoint - vCurrentPosition;
	Vec3 vRightDir = Vec3::Up.Cross(vLookDir);
	Vec3 vUpDir = vLookDir.Cross(vRightDir);
	vLookDir.Normalize();
	vRightDir.Normalize();
	vUpDir.Normalize();

	Set_Info(TRANSFORM_INFO_STATE::RIGHT, vRightDir * vScale.x);
	Set_Info(TRANSFORM_INFO_STATE::UP, vUpDir * vScale.y);
	Set_Info(TRANSFORM_INFO_STATE::LOOK, vLookDir * vScale.z);
}

inline void CTransform::Chase(const Vec3& vPoint, _float fMinDistance, const _float fTimeDelta, CNavigation* pNavigation)
{
	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetDir = vPoint - vPosition;
	_float fLength = vTargetDir.Length();
	vTargetDir.Normalize();

	if (fLength >= fMinDistance)
		vPosition += vTargetDir * m_fMovePerSec * m_fMoveScale * fTimeDelta;

	if (pNavigation == nullptr || pNavigation->Is_Move(vPosition))
		Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
}

void CTransform::Start_Force(Vec3 vTargetDir, _float fForceAbs, _float fDragK)
{
	if (fForceAbs <= 0.f || fDragK <= 0.f)
		return;

	vTargetDir.Normalize();
	m_vForceVelocity = vTargetDir * fForceAbs;
	m_fDragK = fDragK;
}

void CTransform::Apply_Force(_float fDeltaTime, CNavigation* pNavigation)
{
	if (m_fDragK <= 0.f)
		return;

	Vec3 vPosition = Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vVelocity = m_vForceVelocity;
	Vec3 vNext = vPosition + vVelocity * fDeltaTime;

	if (pNavigation == nullptr || pNavigation->Is_Move(vNext))
		Set_Info(TRANSFORM_INFO_STATE::POS, vNext);

	_float fDecay = std::exp(-m_fDragK * fDeltaTime);
	vVelocity *= fDecay;

	_float fSpeedSq = vVelocity.LengthSquared();
	if (fSpeedSq < 0.05f * 0.05f)
		Force_Clear();
	else
		m_vForceVelocity = vVelocity;
}

void CTransform::Update_PrevPosition()
{
	m_vPrevPosition =
		Vec3(
			m_matWorld.m[ENUM_TO_UINT(TRANSFORM_INFO_STATE::POS)][0],
			m_matWorld.m[ENUM_TO_UINT(TRANSFORM_INFO_STATE::POS)][1],
			m_matWorld.m[ENUM_TO_UINT(TRANSFORM_INFO_STATE::POS)][2]
		);
}

void CTransform::Force_Clear()
{
	m_vForceVelocity = { 0.f, 0.f, 0.f };
	m_fDragK = { 0.f };
}

void CTransform::MoveArgWorld_ToMyWorld(Matrix& vNewWorld, _bool isChangeThis)
{
	vNewWorld *= m_matWorld;

	if (isChangeThis)
		m_matWorld = vNewWorld;
}

void CTransform::MoveMyWorld_ToArgWorld(Matrix& vNewWorld, _bool isChangeArg)
{
	m_matWorld *= vNewWorld;

	if (isChangeArg)
		vNewWorld = m_matWorld;
}

void CTransform::Add_Position(const Vec3& vAddPos)
{
	m_matWorld.Translation(Get_Info(TRANSFORM_INFO_STATE::POS) + vAddPos);
}

Vec3 CTransform::LocalPos_toMyWorld(const Vec3& vLocalPos, _bool bDir)
{
	if (bDir)
	{
		return Vec3::TransformNormal(vLocalPos, m_matWorld);
	}

	else
		return Vec3::Transform(vLocalPos, m_matWorld);
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