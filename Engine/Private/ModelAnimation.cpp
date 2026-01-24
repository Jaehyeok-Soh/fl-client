#include "Engine_pch.h"
#include "ModelAnimation.h"
#include "Channel.h"

CModelAnimation::CModelAnimation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EResourceType::MODEL_ANIMATION, pDevice, pDeviceContext)
{
}

CModelAnimation::CModelAnimation(const CModelAnimation& rhs)
	: Super(rhs)
	, m_iChannelCount(rhs.m_iChannelCount)
	, m_vecChannels(rhs.m_vecChannels)
	, m_vecCurrentKeyFrameIndices(rhs.m_vecCurrentKeyFrameIndices)
	, m_fCurrentTrackPosition(rhs.m_fCurrentTrackPosition)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fDuration(rhs.m_fDuration)
{
	for (auto& pElement : m_vecChannels)
		Safe_AddRef(pElement);
}

HRESULT CModelAnimation::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	MODELANIM_DESC* pDesc = static_cast<MODELANIM_DESC*>(pArg);
	m_fDuration = pDesc->fDuration;
	m_fTickPerSecond = pDesc->fTickPerSecond;
	m_iChannelCount = (_uint)pDesc->spanChannels.size();
	m_vecCurrentKeyFrameIndices.resize(m_iChannelCount);
	if (m_iChannelCount > 0)
	{
		m_vecChannels.resize(m_iChannelCount);
		::memcpy(m_vecChannels.data(), pDesc->spanChannels.data(), sizeof(CChannel*) * m_iChannelCount);
	}
	else
		return E_FAIL;

	return S_OK;
}

_bool CModelAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (!isLoop)
			return true;

		m_fCurrentTrackPosition = 0.f;
	}

	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++]);
	}
	return false;
}

void CModelAnimation::SetUp_PoseDatasForBlending(std::span<LOCALSRT> spanLocalSrtData, _float fTimeDelta)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = m_fDuration;
	}
	
	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		pChannel->SetUp_PoseData(spanLocalSrtData, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++]);
	}
}

void CModelAnimation::Clear()
{
	m_fCurrentTrackPosition = 0.f;
	std::fill(m_vecCurrentKeyFrameIndices.begin(), m_vecCurrentKeyFrameIndices.end(), 0);
}

_bool CModelAnimation::Is_TrackPositionBetween(_float fStartRatio, _float fEndRatio)
{
	return Is_TrackPositionAt(fStartRatio) && (Is_TrackPositionAt(fEndRatio) == false);
}

CModelAnimation* CModelAnimation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CModelAnimation* pInstance = new CModelAnimation(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CModelAnimation::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CModelAnimation* CModelAnimation::Clone()
{
	return new CModelAnimation(*this);
}

void CModelAnimation::Free()
{
	for (auto& pElement : m_vecChannels)
		Safe_Release(pElement);
	m_vecChannels.clear();
	Super::Free();
}
