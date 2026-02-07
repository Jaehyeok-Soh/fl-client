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

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta)
{
	if (fCurrentTrackPosition <= 0.f)
		*pCurrentKeyFrameIndex = 0;

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
			Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
			vLeftTranslation	= { 0.f,0.f,0.f };
			vRightTranslation	= { 0.f,0.f,0.f };
		}

		vScale = Vec3::Lerp(vLeftScale, vRightScale, fRatio);
		vQuaternion = Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation = Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
	vecBones[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);
}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta)
{
	if (fCurrentTrackPosition <= 0.f)
		*pCurrentKeyFrameIndex = 0;

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

		vLeftScale			= m_vecKeyframes[(*pCurrentKeyFrameIndex)].vScale;
		vRightScale			= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vScale;

		vLeftQuaternion		= m_vecKeyframes[(*pCurrentKeyFrameIndex)].vQuaterion;
		vRightQuaternion	= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vQuaterion;

		vLeftTranslation	= m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation;
		vRightTranslation	= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation;


		_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

		if (m_bRootBone)
		{
			Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
			vLeftTranslation	= { 0.f,0.f,0.f };
			vRightTranslation	= { 0.f,0.f,0.f };
		}

		vScale			= Vec3::Lerp(vLeftScale, vRightScale, fRatio);
		vQuaternion		= Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation	= Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	spanLocalSrtData[m_iBoneIndex].vScale		= vScale;
	spanLocalSrtData[m_iBoneIndex].vQuaterion	= vQuaternion;
	spanLocalSrtData[m_iBoneIndex].vTranslation = vTranslation;
}

void CChannel::Move_OnwerTransform(_float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta)
{
	if (m_bRootBone)
	{
		if (fCurrentTrackPosition <= 0.f)
			*pCurrentKeyFrameIndex = 0;

		if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);
		
		Vec3 vLeftTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation;
		Vec3 vRightTranslation = m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		Update_MotionBone(vLeftTranslation, vRightTranslation, pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
	}
}

void CChannel::Update_MotionBone(Vec3 vLeftTrans, Vec3 vRightTrans,  CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, const _float fTimeDelta)
{
	if (pOwnerTransform == nullptr ||
		pOwnerPhyCCT == nullptr)
		return;

	/* 좌표계 차이 때문에 x,z,y & 부호 반대로 */
	_float fLocalRight	= vLeftTrans.x - vRightTrans.x;
	_float fLocalUp		= vLeftTrans.z - vRightTrans.z;
	_float fLocalLook	= vLeftTrans.y - vRightTrans.y;

	if (fLocalRight == 0
		&& fLocalUp == 0
		&& fLocalLook == 0)
		return;

	Vec3 vOwnerRight	= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vOwnerUp		= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
	Vec3 vOwnerLook		= pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

	Vec3 moveDistance = vOwnerRight * fLocalRight + vOwnerUp * fLocalUp + vOwnerLook * fLocalLook;

	pOwnerPhyCCT->Move(moveDistance, 0.0f, fTimeDelta);

	Vec3 finalPos = pOwnerPhyCCT->GetFootPosition();

	pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::POS, finalPos);
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

void CChannel::Free()
{
	Super::Free();
}
