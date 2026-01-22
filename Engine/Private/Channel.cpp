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

	_float4x4 matTransformation = {};
	KEYFRAME lastKeyFrame = m_vecKeyframes.back();
	_vector vScale, vQuaternion, vTranslation;

	if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
	{
		vScale = ::XMLoadFloat3(&lastKeyFrame.vScale);
		vQuaternion = ::XMLoadFloat4(&lastKeyFrame.vQuaterion);
		vTranslation = ::XMVectorSetW(::XMLoadFloat3(&lastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		_vector		vLeftScale{}, vRightScale{};
		_vector		vLeftQuaternion{}, vRightQuaternion{};
		_vector		vLeftTranslation{}, vRightTranslation{};

		if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		vLeftScale = ::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vScale);
		vRightScale = ::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vScale);

		vLeftQuaternion = ::XMLoadFloat4(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vQuaterion);
		vRightQuaternion = ::XMLoadFloat4(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vQuaterion);

		vLeftTranslation = ::XMVectorSetW(::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
		vRightTranslation = ::XMVectorSetW(::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);

		_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = ::XMVectorLerp(vLeftScale, vRightScale, fRatio);
		vQuaternion = ::XMQuaternionSlerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation = ::XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	::XMStoreFloat4x4(&matTransformation,
		::XMMatrixAffineTransformation(vScale, ::XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation));

	vecBones[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);
}

void CChannel::SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	if (fCurrentTrackPosition <= 0.f)
		*pCurrentKeyFrameIndex = 0;

	_float4x4 matTransformation = {};
	KEYFRAME lastKeyFrame = m_vecKeyframes.back();
	_vector vScale, vQuaternion, vTranslation;

	if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
	{
		vScale = ::XMLoadFloat3(&lastKeyFrame.vScale);
		vQuaternion = ::XMLoadFloat4(&lastKeyFrame.vQuaterion);
		vTranslation = ::XMVectorSetW(::XMLoadFloat3(&lastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		_vector		vLeftScale{}, vRightScale{};
		_vector		vLeftQuaternion{}, vRightQuaternion{};
		_vector		vLeftTranslation{}, vRightTranslation{};

		if (fCurrentTrackPosition >= m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		vLeftScale = ::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vScale);
		vRightScale = ::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vScale);

		vLeftQuaternion = ::XMLoadFloat4(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vQuaterion);
		vRightQuaternion = ::XMLoadFloat4(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vQuaterion);

		vLeftTranslation = ::XMVectorSetW(::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
		vRightTranslation = ::XMVectorSetW(::XMLoadFloat3(&m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);

		_float		fRatio = (fCurrentTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_vecKeyframes[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_vecKeyframes[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = ::XMVectorLerp(vLeftScale, vRightScale, fRatio);
		vQuaternion = ::XMQuaternionSlerp(vLeftQuaternion, vRightQuaternion, fRatio);
		vTranslation = ::XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	::XMStoreFloat3(&spanLocalSrtData[m_iBoneIndex].vScale, vScale);
	::XMStoreFloat4(&spanLocalSrtData[m_iBoneIndex].vQuaterion, vQuaternion);
	::XMStoreFloat3(&spanLocalSrtData[m_iBoneIndex].vTranslation, vTranslation);
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
