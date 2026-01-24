#include "Engine_pch.h"
#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNEL_DESC& desc)
{

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

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
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

		vScale = Vec3::Lerp(vLeftScale, vRightScale, fRatio);
		vQuaternion = Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation = Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
	vecBones[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);
}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
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

		vScale = Vec3::Lerp(vLeftScale, vRightScale, fRatio);
		vQuaternion = Quat::Slerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation = Vec3::Lerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	spanLocalSrtData[m_iBoneIndex].vScale = vScale;
	spanLocalSrtData[m_iBoneIndex].vQuaterion = vQuaternion;
	spanLocalSrtData[m_iBoneIndex].vTranslation = vTranslation;
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
