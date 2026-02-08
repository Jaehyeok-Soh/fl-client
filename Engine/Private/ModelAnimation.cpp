#include "Engine_pch.h"
#include "ModelAnimation.h"
#include "Channel.h"

#include "Transform.h"
#include "ComputeShader.h"
#include "StructuredBuffer.h"

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

	MODELANIM_DESC* pDesc	= static_cast<MODELANIM_DESC*>(pArg);
	m_iRootBoneIdx			= pDesc->iRootBondIndex;
	m_fDuration				= pDesc->fDuration;
	m_fTickPerSecond		= pDesc->fTickPerSecond;
	m_iChannelCount			= (_uint)pDesc->spanChannels.size();
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

_bool CModelAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform,  CPhysicsCCT* pOwnerPhyCCT)
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
		pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
	}
	return false;
}

void CModelAnimation::SetUp_PoseDatasForBlending(std::span<LOCALSRT> spanLocalSrtData, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = m_fDuration;
	}
	
	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		pChannel->SetUp_PoseData(spanLocalSrtData, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
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

_bool CModelAnimation::Update_TransformMatrices(CComputeShader* pAnimEShader, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	// track 계산
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (!isLoop)
			return true;

		m_fCurrentTrackPosition = 0.f;
	}

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;

	pAnimEShader->Bind_Compute_Track(tMuDesc);
	
	// dispatch
	_uint iGroupX = (m_iChannelCount + 31) / 32;
	pAnimEShader->Dispatch(iGroupX, 1, 1);


	//m_iRootChannelIdx
	if(m_iRootChannelIdx> 0)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
}

void CModelAnimation::Update_BlendAnimation(CComputeShader* pAnimEShader, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	//내 애니메이션 정보 전달
	Bind_AnimationEData(pAnimEShader);

	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = m_fDuration;
	}

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;

	pAnimEShader->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (m_iChannelCount + 31) / 32;
	pAnimEShader->Dispatch(iGroupX, 1, 1);

	//m_iRootChannelIdx
	if (m_iRootChannelIdx > 0)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
}

void CModelAnimation::Bind_AnimationEData(CComputeShader* pAnimEShader)
{
	//pAnimEShader->m_pKeyFrameBuffer 할당
	//pAnimEShader->m_pChannelDataBuffer 할당
}

HRESULT CModelAnimation::Ready_Buffers(CComputeShader* pAnimESahder)
{
	// 1. 버퍼의 사이즈를 할당한다
	m_iChannelSize = m_vecChannels.size();
	m_iKeyFrameBufferSize = 0;
	for (auto& pChannel : m_vecChannels)
	{
		m_iKeyFrameBufferSize += pChannel->Get_KeyFrames().size();
	}

	CS_IMMU_ANIM_KEYFRAME* pIniailKeyData			= new CS_IMMU_ANIM_KEYFRAME[m_iKeyFrameBufferSize];
	CS_IMMU_ANIM_CHANNELDATA* pIniailChannelData	= new CS_IMMU_ANIM_CHANNELDATA[m_iChannelSize];

	// 2. 버퍼 내용을 쓴다
	for (size_t i = 0 ; i < m_vecChannels.size() ; i++)
	{
		vector<KEYFRAME> KeyFrames = m_vecChannels[i]->Get_KeyFrames();

		// 2.1 key frame은 그대로 받기
		for (auto& KeyFrame : KeyFrames)
		{
			pIniailKeyData[i].vScale = KeyFrame.vScale;
			pIniailKeyData[i].vQuat = KeyFrame.vQuaterion;
			pIniailKeyData[i].vTranslation = KeyFrame.vTranslation;
			pIniailKeyData[i].fTrackPosition = KeyFrame.fTrackPosition;
			pIniailKeyData[i].fPadding0 = 0.f;
		}

		// 2.2 채널당 정보이므로 여기서 작성
		pIniailChannelData[i].iBoneIndex = m_vecChannels[i]->Get_BoneIndex();
		pIniailChannelData[i].iKeyStart = i;
		pIniailChannelData[i].iKeyCount = KeyFrames.size();
		pIniailChannelData[i].iBoneIndex = m_iRootBoneIdx;

		// 2.3 root channel 캐싱
		if (i == m_iRootBoneIdx)
			m_iRootChannelIdx = i;
	}

	// 3. struct buffer class 생성
	m_pKeyFrameBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_IMMU_ANIM_KEYFRAME), m_iKeyFrameBufferSize);
	m_pChannelDataBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_IMMU_ANIM_CHANNELDATA), m_iChannelSize);

	// 4. buffer에 값 넣어줌
	m_pKeyFrameBuffer->Copy_Data(pIniailKeyData, sizeof(CS_IMMU_ANIM_KEYFRAME), m_iKeyFrameBufferSize);
	m_pChannelDataBuffer->Copy_Data(pIniailChannelData, sizeof(CS_IMMU_ANIM_CHANNELDATA), m_iChannelSize);

	// 4. SRV 연결
	m_pInputKeySB_SRV = pAnimESahder->Get_SRV("IMMU_KEYFRAMS");
	m_pInputKeySB_SRV->SetResource(m_pKeyFrameBuffer->Get_SRV());

	m_pInputChannelSB_SRV = pAnimESahder->Get_SRV("IMMU_CHANNELDATAS");
	m_pInputChannelSB_SRV->SetResource(m_pChannelDataBuffer->Get_SRV());
	
	// 5. 동적배열 정리
	Safe_Delete_Array(pIniailKeyData);
	Safe_Delete_Array(pIniailChannelData);

	if (m_pKeyFrameBuffer == nullptr ||
		m_pChannelDataBuffer == nullptr)
		return E_FAIL;

	return S_OK;
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

	Safe_Release(m_pKeyFrameBuffer);
	Safe_Release(m_pChannelDataBuffer);
	Safe_Release(m_pInputKeySB_SRV);
	Safe_Release(m_pInputChannelSB_SRV);

	Super::Free();
}
