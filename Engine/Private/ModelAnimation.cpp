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
	, m_pKeyFrameBuffer(rhs.m_pKeyFrameBuffer)
	, m_pInputKeySB_SRV(rhs.m_pInputKeySB_SRV)
	, m_pChannelDataBuffer(rhs.m_pChannelDataBuffer)
	, m_iKeyFrameBufferSize(rhs.m_iKeyFrameBufferSize)
	, m_iChannelSize(rhs.m_iChannelSize)
	, m_iRootBoneIdx(rhs.m_iRootBoneIdx)
	, m_iRootChannelIdx(rhs.m_iRootChannelIdx)
	, m_bApplyRootMotion(rhs.m_bApplyRootMotion)
{
	Safe_AddRef(m_pKeyFrameBuffer);
	//Safe_AddRef(m_pInputKeySB_SRV);

	Safe_AddRef(m_pChannelDataBuffer);
	//Safe_AddRef(m_pInputChannelSB_SRV);

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

	if (m_iRootBoneIdx < 0)
		m_bApplyRootMotion = false;

	//if((_wstring)Get_Name() == )
	//	m_bApplyRootMotion = false;

	return S_OK;
}

_bool CModelAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform,  CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimECS)
{
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
	tMuDesc.iChannelCount = m_iChannelCount;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (_uint(vecBones.size()) + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);

	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);
	}
	return false;
}

void CModelAnimation::SetUp_PoseDatasForBlending(std::span<LOCALSRT> spanLocalSrtData, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT,_uint iTotalBoneNum, CComputeShader* pAnimECS)
{
	//내 애니메이션 정보 전달
	Bind_AnimationEData(pAnimECS);

	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = m_fDuration;
	}

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;
	tMuDesc.iChannelCount = m_iChannelCount;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);
	
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

_bool CModelAnimation::Update_TransformMatrices(CComputeShader* pAnimECS,_float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, _uint iTotalBoneNum)
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
	tMuDesc.iChannelCount = m_iChannelCount;
	pAnimECS->Bind_Compute_Track(tMuDesc);
	
	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);

	//m_iRootChannelIdx
	if(m_bApplyRootMotion)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);

	return false;
}

void CModelAnimation::Update_BlendAnimation(CComputeShader* pAnimECS, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, _uint iTotalBoneNum)
{
	//내 애니메이션 정보 전달
	Bind_AnimationEData(pAnimECS);

	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = m_fDuration;
	}

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;
	tMuDesc.iChannelCount = m_iChannelCount;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);

	//m_iRootChannelIdx
	if (m_bApplyRootMotion)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta);

	//else
	//	int test = 0;
}

void CModelAnimation::Bind_AnimationEData(CComputeShader* pAnimEShader)
{
	pAnimEShader->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_KEYFRAME), m_pInputKeySB_SRV, m_pKeyFrameBuffer);
	pAnimEShader->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_CHANNELDATA), m_pInputChannelSB_SRV, m_pChannelDataBuffer);
}

HRESULT CModelAnimation::Ready_Buffers()
{
	// 1. 버퍼의 사이즈를 할당한다
	m_iChannelSize = _uint(m_vecChannels.size());
	m_iKeyFrameBufferSize = 0;
	for (auto& pChannel : m_vecChannels)
	{
		m_iKeyFrameBufferSize += _uint(pChannel->Get_KeyFrames().size());
	}

	// 3. struct buffer class 생성
	m_pKeyFrameBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_IMMU_ANIM_KEYFRAME), m_iKeyFrameBufferSize);
	m_pChannelDataBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_IMMU_ANIM_CHANNELDATA), m_iChannelSize);
	
	if (m_pKeyFrameBuffer == nullptr ||
		m_pChannelDataBuffer == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CModelAnimation::Ready_BindBuffers(CComputeShader* pAnimESahder)
{
	if (FAILED(Ready_Buffers()))
		return E_FAIL;

	CS_IMMU_ANIM_KEYFRAME* pIniailKeyData = new CS_IMMU_ANIM_KEYFRAME[m_iKeyFrameBufferSize];
	CS_IMMU_ANIM_CHANNELDATA* pIniailChannelData = new CS_IMMU_ANIM_CHANNELDATA[m_iChannelSize];

	_uint iKeyAcc = {};

	// 2. 버퍼 내용을 쓴다
	for (size_t i = 0; i < m_vecChannels.size(); i++)
	{
		vector<KEYFRAME> KeyFrames = m_vecChannels[i]->Get_KeyFrames();

		// 2.1 key frame은 그대로 받기
		for(size_t j =  0 ; j< KeyFrames.size() ; j++)
		{
			pIniailKeyData[iKeyAcc + j].vScale = KeyFrames[j].vScale;
			pIniailKeyData[iKeyAcc + j].vQuat = KeyFrames[j].vQuaterion;
			pIniailKeyData[iKeyAcc + j].vTranslation = KeyFrames[j].vTranslation;
			pIniailKeyData[iKeyAcc + j].fTrackPosition = KeyFrames[j].fTrackPosition;
			pIniailKeyData[iKeyAcc + j].fPadding0 = 0.f;
		}

		// 2.2 채널당 정보이므로 여기서 작성
		pIniailChannelData[i].iBoneIndex = m_vecChannels[i]->Get_BoneIndex();


		// 여기 걸리면 model cs 수정 필요
		if (m_vecChannels[i]->Get_BoneIndex() < 0)
		{
			MSG_BOX("Warnning : Channel Bone Index < 0");
		}

		if (KeyFrames.size() == 1)
		{
			MSG_BOX("Warnning : Channel Frame Has One!!!!!");
		}

		pIniailChannelData[i].iKeyStart = iKeyAcc;
		pIniailChannelData[i].iKeyCount = _uint(KeyFrames.size());
		pIniailChannelData[i].iRootMotionBoneIndex = m_iRootBoneIdx;

		iKeyAcc += _uint(KeyFrames.size());

		// 2.3 root channel 캐싱
		if (m_vecChannels[i]->Get_BoneIndex() == m_iRootBoneIdx)
			m_iRootChannelIdx = (_uint)i;
	}

	// 4. buffer에 값 넣어줌
	m_pKeyFrameBuffer->Copy_Data(pIniailKeyData, sizeof(CS_IMMU_ANIM_KEYFRAME), m_iKeyFrameBufferSize);
	m_pChannelDataBuffer->Copy_Data(pIniailChannelData, sizeof(CS_IMMU_ANIM_CHANNELDATA), m_iChannelSize);

	// 5. 동적배열 정리
	Safe_Delete_Array(pIniailKeyData);
	Safe_Delete_Array(pIniailChannelData);

	if (m_pKeyFrameBuffer == nullptr ||
		m_pChannelDataBuffer == nullptr)
		return E_FAIL;

	// 4. SRV 연결
	m_pInputKeySB_SRV = pAnimESahder->Get_SRV("IMMU_KEYFRAMS");
	m_pInputKeySB_SRV->SetResource(m_pKeyFrameBuffer->Get_SRV());

	m_pInputChannelSB_SRV = pAnimESahder->Get_SRV("IMMU_CHANNELDATAS");
	m_pInputChannelSB_SRV->SetResource(m_pChannelDataBuffer->Get_SRV());

	if (m_pInputKeySB_SRV == nullptr ||
		m_pInputChannelSB_SRV == nullptr)
		return E_FAIL;

	return S_OK;
}

void CModelAnimation::Check_UpdateCpu(const vector<class CBone*>& vecBones)
{
	for (auto& pChannel : m_vecChannels)
	{
		pChannel->Check_UpdateCpu(vecBones);
	}
}

void CModelAnimation::Set_Notifies(vector<AnimNotifyKey> vecKeys)
{
	std::sort(vecKeys.begin(), vecKeys.end(),
		[](const AnimNotifyKey& a, const AnimNotifyKey& b)->_bool
		{
			return a.fTrackPosition < b.fTrackPosition;
		});
	m_vecNotifies = std::move(vecKeys);
	m_iNextNotifyIndex = 0;
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

	if (!IsClone())
	{
		Safe_Release(m_pInputKeySB_SRV);
		Safe_Release(m_pInputChannelSB_SRV);
	}

	Super::Free();
}
