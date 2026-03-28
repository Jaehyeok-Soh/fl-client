#include "Engine_pch.h"
#include "CameraController.h"
#include "CameraModifier_Shake.h"
#include "CameraModifier_FOV.h"
#include "CameraModifier_PositionOffset.h"
#include "CameraModifier_RotationOffset.h"
#include "GameInstance.h"

CCameraController::CCameraController()
	: Super()
{
}

CCameraController::CCameraController(const CCameraController& rhs)
	: Super(rhs)
	, m_tConfig(rhs.m_tConfig)
{
}

HRESULT CCameraController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
		m_tConfig = *static_cast<CAMERA_CONTROLLER_DESC*>(pArg);

	return S_OK;
}

void CCameraController::Update_Controller(_float fTimeDelta)
{
	for (auto& pModifier : m_vecModifiers)
	{
		if (pModifier)
			pModifier->Update(fTimeDelta);
	}

	Remove_FinishedModifiers();
}

void CCameraController::Play_Shake(const CAMERA_SHAKE_DESC& desc)
{
	Add_Modifier(Create_ShakeModifier(desc));
}

// Fov는 중복 금지, 최신것만 유지
void CCameraController::Play_FOV(const CAMERA_FOV_DESC& desc)
{
	Remove_ModifiersByType(ECameraModifierType::Fov);
	Add_Modifier(Create_FOVModifier(desc));
}

void CCameraController::Play_PositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc)
{
	Add_Modifier(Create_PositionOffsetModifier(desc));
}

void CCameraController::Play_RotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc)
{
	Add_Modifier(Create_RotationOffsetModifier(desc));
}

void CCameraController::Stop_All()
{
	for (auto& pModifier : m_vecModifiers)
		Safe_Release(pModifier);

	m_vecModifiers.clear();
}

void CCameraController::Stop_ByType(ECameraModifierType eType)
{
	Remove_ModifiersByType(eType);
}

void CCameraController::Build_FinalPose_FromResult(const CAMERA_POSE& basePose, const CAMERA_MODIFIER_RESULT& tResult, OUT CAMERA_POSE& outPose) const
{
	outPose = basePose;

	//////////////////
	/// 회전 오프셋 ///
	/////////////////
	Matrix matBase = Matrix::Identity;
	matBase.Right(basePose.vRight);
	matBase.Up(basePose.vUp);
	matBase.Backward(basePose.vLook);

	Matrix matRot = Matrix::CreateFromYawPitchRoll(
		tResult.fYawOffsetRad,
		tResult.fPitchOffsetRad,
		tResult.fRollOffsetRad);

	Matrix matFinal = matRot * matBase;

	Vec3 vRight = matFinal.Right();
	Vec3 vUp = matFinal.Up();
	Vec3 vLook = matFinal.Backward();

	vRight.Normalize();
	vUp.Normalize();
	vLook.Normalize();

	outPose.vRight = vRight;
	outPose.vUp = vUp;
	outPose.vLook = vLook;

	//////////////////
	/// 로컬 오프셋 ///
	/////////////////
	outPose.vPos += vRight * tResult.vLocalPosOffset.x;
	outPose.vPos += vUp * tResult.vLocalPosOffset.y;
	outPose.vPos += vLook * tResult.vLocalPosOffset.z;

	//////////////////
	/// 월드 오프셋 ///
	/////////////////
	outPose.vPos += tResult.vWorldPosOffset;

	///////////
	/// Fov ///
	///////////
	outPose.fFovRad = basePose.fFovRad + tResult.fFovDeltaRad;
	outPose.fFovRad = std::clamp(outPose.fFovRad, m_tConfig.fMinFovRad, m_tConfig.fMaxFovRad);
}

void CCameraController::Build_FinalPose_WithOverlay(const CAMERA_POSE& basePose, const CAMERA_MODIFIER_RESULT& tOverlay, OUT CAMERA_POSE& outPose) const
{
	CAMERA_MODIFIER_RESULT tTotal{};
	Accumulate_Modifiers(basePose, tTotal);
	Accumulate_Result(tTotal, tOverlay);

	Build_FinalPose_FromResult(basePose, tTotal, outPose);
}

void CCameraController::Build_FinalPose(const CAMERA_POSE& basePose, OUT CAMERA_POSE& outPose) const
{
	CAMERA_MODIFIER_RESULT tResult{};
	Accumulate_Modifiers(basePose, tResult);
	Build_FinalPose_FromResult(basePose, tResult, outPose);
}

void CCameraController::Add_Modifier(ICameraModifier* pModifier)
{
	if (pModifier == nullptr)
		return;

	pModifier->Start();
	m_vecModifiers.push_back(pModifier);
}

void CCameraController::Remove_FinishedModifiers()
{
	for (auto it = m_vecModifiers.begin(); it != m_vecModifiers.end(); )
	{
		ICameraModifier* pModifier = *it;
		if (pModifier == nullptr || pModifier->IsFinished())
		{
			Safe_Release(pModifier);
			it = m_vecModifiers.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CCameraController::Remove_ModifiersByType(ECameraModifierType eType)
{
	for (auto it = m_vecModifiers.begin(); it != m_vecModifiers.end(); )
	{
		ICameraModifier* pModifier = *it;
		if (pModifier && pModifier->Get_Type() == eType)
		{
			Safe_Release(pModifier);
			it = m_vecModifiers.erase(it);
		}
		else
		{
			++it;
		}
	}
}

ICameraModifier* CCameraController::Create_ShakeModifier(const CAMERA_SHAKE_DESC& desc)
{
	return CCameraModifier_Shake::Create(desc);
}

ICameraModifier* CCameraController::Create_FOVModifier(const CAMERA_FOV_DESC& desc)
{
	return CCameraModifier_Fov::Create(desc);
}

ICameraModifier* CCameraController::Create_PositionOffsetModifier(const CAMERA_POSITION_OFFSET_DESC& desc)
{
	return CCameraModifier_PositionOffset::Create(desc);;
}

ICameraModifier* CCameraController::Create_RotationOffsetModifier(const CAMERA_ROTATION_OFFSET_DESC& desc)
{
	return CCameraModifier_RotationOffset::Create(desc);
}

void CCameraController::Accumulate_Modifiers(const CAMERA_POSE& basePose, CAMERA_MODIFIER_RESULT& outResult) const
{
	outResult = {};

	for (auto& pModifier : m_vecModifiers)
	{
		if (pModifier)
			pModifier->Accumulate(basePose, outResult);
	}
}

void CCameraController::Accumulate_Result(IN OUT CAMERA_MODIFIER_RESULT& ioResult, const CAMERA_MODIFIER_RESULT& tAdd) const
{
	ioResult.vLocalPosOffset += tAdd.vLocalPosOffset;
	ioResult.vWorldPosOffset += tAdd.vWorldPosOffset;

	ioResult.fYawOffsetRad += tAdd.fYawOffsetRad;
	ioResult.fPitchOffsetRad += tAdd.fPitchOffsetRad;
	ioResult.fRollOffsetRad += tAdd.fRollOffsetRad;
	ioResult.fFovDeltaRad += tAdd.fFovDeltaRad;
}

CCameraController* CCameraController::Create()
{
	CCameraController* pInstance = new CCameraController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CCameraController::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CCameraController::Clone(void* pArg)
{
	CCameraController* pInstance = new CCameraController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CCameraController::Clone(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CCameraController::Free()
{
	Stop_All();
	Super::Free();
}