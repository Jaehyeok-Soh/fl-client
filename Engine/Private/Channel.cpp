#include "Engine_pch.h"
#include "Channel.h"

#include "Bone.h"
#include "Transform.h"
#include "PhysicsCCT.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNEL_DESC& desc)
{
	m_matTrans = {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
	};


	::strcpy_s(m_szName, desc.strName.c_str());
	m_iBoneIndex = desc.iBoneIndex;
	m_iKeyFrameCount = (_uint)desc.spanKeyframes.size();
	if (m_iKeyFrameCount > 0)
	{
		m_vecKeyframes.resize(m_iKeyFrameCount);
		::memcpy(m_vecKeyframes.data(), desc.spanKeyframes.data(), sizeof(KEYFRAME) * m_iKeyFrameCount);
	}
	else
		return E_FAIL;

	m_bRootBone = (desc.iRootBoneIndex == m_iBoneIndex);


	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, _float fMotionOffset)
{
}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, _float fMotionOffset)
{
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta, _float fMotionOffset, OUT Vec3& vPrepos)
{
	if (m_bUpdateCpu)
	{
		if (fCurrentTrackPosition <= 0.f)
		{
			*pCurrentKeyFrameIndex = 0;
			if(m_bRootBone)
				Reset_PreTranslation(vPrepos);
		}


		Matrix matTransformation = {};
		KEYFRAME lastKeyFrame = m_vecKeyframes.back();
		Vec3 vScale, vTranslation;
		Quat vQuaternion;
		if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
		{
			vScale = lastKeyFrame.vScale;
			vQuaternion = lastKeyFrame.vQuaterion;
			vTranslation = lastKeyFrame.vTranslation;
		}
		else
		{
			Vec3		vLeftScale{}, vRightScale{};
			Quat		vLeftQuaternion{}, vRightQuaternion{};
			Vec3		vLeftTranslation{}, vRightTranslation{};

			if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
				++(*pCurrentKeyFrameIndex);

			vLeftScale = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vScale;
			vRightScale = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vScale;

			vLeftQuaternion = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vQuaterion;
			vRightQuaternion = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vQuaterion;

			vLeftTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation;
			vRightTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation;

			_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
				(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

			if (m_bRootBone)
			{
				Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta, fMotionOffset, fRatio, vPrepos);
				vLeftTranslation = { 0.f,0.f,0.f };
				vRightTranslation = { 0.f,0.f,0.f };
			}

			vScale = Vec3::Lerp(vLeftScale, vRightScale, fRatio);
			vQuaternion = Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
			vTranslation = Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
		}

		matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
		vecBones[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);
	}

	else if (m_bRootBone)
		Move_OnwerTransform(fCurrentTrackPosition, pCurrentKeyFrameIndex, pOwnerTransform, pOwnerPhyCCT, fTimeDelta, fMotionOffset, vPrepos);

}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta, _float fMotionOffset, OUT Vec3& vPrepos)
{
	if (m_bUpdateCpu)
	{
		if (fCurrentTrackPosition <= 0.f)
		{
			*pCurrentKeyFrameIndex = 0;
			Reset_PreTranslation(vPrepos);
		}

		Matrix matTransformation = {};
		KEYFRAME lastKeyFrame = m_vecKeyframes.back();
		Vec3 vScale, vTranslation;
		Quat vQuaternion;

		if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
		{
			vScale = lastKeyFrame.vScale;
			vQuaternion = lastKeyFrame.vQuaterion;
			vTranslation = lastKeyFrame.vTranslation;
		}
		else
		{
			Vec3		vLeftScale{}, vRightScale{};
			Quat		vLeftQuaternion{}, vRightQuaternion{};
			Vec3		vLeftTranslation{}, vRightTranslation{};

			if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
				++(*pCurrentKeyFrameIndex);

			vLeftScale = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vScale;
			vRightScale = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vScale;

			vLeftQuaternion = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vQuaterion;
			vRightQuaternion = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vQuaterion;

			vLeftTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation;
			vRightTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation;


			_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
				(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

			if (m_bRootBone)
			{
				//Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta, fMotionOffset, fRatio, vPrepos);
				//vLeftTranslation = { 0.f,0.f,0.f };
				//vRightTranslation = { 0.f,0.f,0.f };
			}

			vScale = Vec3::Lerp(vLeftScale, vRightScale, fRatio);
			vQuaternion = Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
			vTranslation = Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
		}

		spanLocalSrtData[m_iBoneIndex].vScale = vScale;
		spanLocalSrtData[m_iBoneIndex].vQuaterion = vQuaternion;
		spanLocalSrtData[m_iBoneIndex].vTranslation = vTranslation;
	}

	else if (m_bRootBone)
		Move_OnwerTransform(fCurrentTrackPosition, pCurrentKeyFrameIndex, pOwnerTransform, pOwnerPhyCCT, fTimeDelta, fMotionOffset, vPrepos);

}

void CChannel::Move_OnwerTransform(_float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta, _float fMotionOffset, OUT Vec3& vPrepos)
{
	//if (m_bRootBone)
	{
		if (fCurrentTrackPosition <= 0.f)
		{
			*pCurrentKeyFrameIndex = 0;
			Reset_PreTranslation(vPrepos);
		}

		KEYFRAME lastKeyFrame = m_vecKeyframes.back();
		if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
		{
			return;
		}

		if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);
		
		Vec3 vLeftTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation;
		Vec3 vRightTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

		Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta, fMotionOffset, fRatio, vPrepos);
	}
}

void CChannel::Update_Addtive(const vector<class CBone*>& vecBones, const vector<KEYFRAME>& RefKeyFrame, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta, _float fMixRatio)
{
	if (m_bUpdateCpu)
	{
		Matrix matPreTransform = vecBones[m_iBoneIndex]->Get_Transform();

		// 0. 필요한 SQT 구하기

		// 0.1 Base SQT
		Vec3 vBaseScale, vBaseTranslation;
		Quat vBaseQuat;
		matPreTransform.Decompose(vBaseScale, vBaseQuat, vBaseTranslation);

		// 0.1 Ref SQT
		Vec3 vRefScale = RefKeyFrame[0].vScale;
		Quat vRefQuat = RefKeyFrame[0].vQuaterion;
		Vec3 vRefTranslation = RefKeyFrame[0].vTranslation;

		// 0.2 My SQT
		Vec3 vScale = m_vecKeyframes[0].vScale;
		Quat vQuat = m_vecKeyframes[0].vQuaterion;
		Vec3 vTranslation = m_vecKeyframes[0].vTranslation;

		KEYFRAME lastKeyFrame = m_vecKeyframes.back();

		if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
		{
			vScale = lastKeyFrame.vScale;
			vQuat = lastKeyFrame.vQuaterion;
			vTranslation = lastKeyFrame.vTranslation;
		}
		else
		{
			if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
				++(*pCurrentKeyFrameIndex);

			auto& left = m_vecKeyframes[*pCurrentKeyFrameIndex];
			auto& right = m_vecKeyframes[*pCurrentKeyFrameIndex + 1];

			float ratio =
				(fCurrentTrackPosition - left.fTrackPosition) /
				(right.fTrackPosition - left.fTrackPosition);

			vScale = Vec3::Lerp(left.vScale, right.vScale, ratio);
			vQuat = Quat::Slerp(left.vQuaterion, right.vQuaterion, ratio);
			vTranslation = Vec3::Lerp(left.vTranslation, right.vTranslation, ratio);
		}


		// 2. delta SQT 구하기
		Vec3 vDeltaScale = vScale / vRefScale;

		Quat InvRefQuat;
		vRefQuat.Conjugate(InvRefQuat);

		Quat DeltaQuat = vQuat * InvRefQuat;

		Vec3 vDeltaTranslation = vTranslation - vRefTranslation;

		// 3. delta SQT에 mixRatio 적용
		Vec3 scaledDelta = Vec3{ 1.f,1.f,1.f } + (vDeltaScale - Vec3{ 1.f,1.f,1.f }) * fMixRatio;

		Quat identity = Quat(0, 0, 0, 1);
		if ((identity).Dot((DeltaQuat)) < 0)
			DeltaQuat = -DeltaQuat;
		Quat scaledDeltaQuat = Quat::Slerp(identity, DeltaQuat, fMixRatio);

		Vec3 scaledDeltaTranslation = vDeltaTranslation * fMixRatio;

		// 4. final SQT 구하기
		Vec3 vFinalScale = vBaseScale * scaledDelta;
		Quat vFinalQuat = (scaledDeltaQuat * vBaseQuat);
		vFinalQuat.Normalize();
		Vec3 vFinalTranslation = vBaseTranslation + scaledDeltaTranslation;
		if (m_bRootBone)
		{
			vFinalTranslation = Vec3(0, 0, 0);
		}

		Matrix matFinal = Matrix::CreateScale(vFinalScale) * Matrix::CreateFromQuaternion(vFinalQuat) * Matrix::CreateTranslation(vFinalTranslation);
		vecBones[m_iBoneIndex]->Set_TransformationMatrix(matFinal);
	}

}

void CChannel::Check_UpdateCpu(const vector<class CBone*>& vecBones)
{
	m_bUpdateCpu = vecBones[m_iBoneIndex]->Get_IsUpdateCpu();
}

_bool CChannel::Set_MotionBone(_int iBoneIdx)
{
	m_bRootBone = (m_iBoneIndex == iBoneIdx);

	return m_bRootBone;
}

void CChannel::Reset_PreTranslation(OUT Vec3& vPrepos)
{
	vPrepos = m_vecKeyframes[0].vTranslation;
}

void CChannel::Update_MotionBone(Vec3 vLeftTrans, Vec3 vRightTrans,  CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta, _float fMotionOffset, _float fRatio, OUT Vec3& vPrepos)
{


	if (pOwnerTransform == nullptr ||
		pOwnerPhyCCT == nullptr)
		return;

	/* 좌표계 차이 때문에 x,z,y & 부호 반대로 */

	//_float fLocalRight	= vLeftTrans.x - vRightTrans.x;
	//_float fLocalUp		= vLeftTrans.z - vRightTrans.z;
	//_float fLocalLook	= vLeftTrans.y - vRightTrans.y;

	Vec3 vLerp = Vec3::Lerp(vLeftTrans, vRightTrans, fRatio);
	Vec3 vDelta = vPrepos - vLerp;
	vPrepos = vLerp;


	//if (fLocalRight == 0
	//	&& fLocalUp == 0
	//	&& fLocalLook == 0)
	//	return;

	Vec3 vOwnerRight	= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vOwnerUp		= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vOwnerLook		= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

	vOwnerRight.Normalize();
	vOwnerUp.Normalize();
	vOwnerLook.Normalize();

	Vec3 moveDistance = vOwnerRight * vDelta.x + vOwnerUp * vDelta.z + vOwnerLook * vDelta.y;

	pOwnerPhyCCT->AddFixedMove(moveDistance * fMotionOffset);
}

CChannel* CChannel::Create(const CHANNEL_DESC& desc)
{
	CChannel* pInstance = new CChannel;
	if (FAILED(pInstance->Initialize(desc)))
	{
		MSG_BOX("CChannel::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Clone()
{
	return new CChannel(*this);

	//::strcpy_s(pInstance->m_szName, m_szName);
	//pInstance->m_iBoneIndex = m_iBoneIndex;
	//pInstance->m_iKeyFrameCount = m_iKeyFrameCount;
	//pInstance->m_vecKeyframes = m_vecKeyframes;  // vector 복사
	//pInstance->m_bRootBone = m_bRootBone;
	//pInstance->m_bUpdateCpu = m_bUpdateCpu;
	//pInstance->m_matTrans = m_matTrans;
	//pInstance->m_vPreRootLocal = m_vPreRootLocal;
}

void CChannel::Free()
{
	Super::Free();
}
