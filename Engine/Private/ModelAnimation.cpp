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
	, m_iKeyFrameBufferSize(rhs.m_iKeyFrameBufferSize)
	, m_iChannelSize(rhs.m_iChannelSize)
	, m_iRootBoneIdx(rhs.m_iRootBoneIdx)
	, m_iRootChannelIdx(rhs.m_iRootChannelIdx)
	, m_bApplyRootMotion(rhs.m_bApplyRootMotion)
	, m_fRootMotionOffset(rhs.m_fRootMotionOffset)
{
	//Safe_AddRef(m_pKeyFrameBuffer);
	//Safe_AddRef(m_pInputKeySB_SRV);

	//Safe_AddRef(m_pChannelDataBuffer);
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

_bool CModelAnimation::Update_TransformationMatrices(const vector<class CBone*>& vecBones, _bool& bLoopDone, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform,  CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimECS)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		// todo_eunbi : 만약 필요하다면 bloopdone = false 해야됨
		if (!isLoop)
			return true;

		else
			bLoopDone = true;

		m_fCurrentTrackPosition = 0.f;
	}

	else
		bLoopDone = false;

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;
	tMuDesc.iChannelCount = m_iChannelCount;
	tMuDesc.iRootMotionBoneIndex = m_iRootBoneIdx;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (_uint(vecBones.size()) + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);


	// 원래 하던대로 channel update
	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		if (iIndex == m_iRootBoneIdx && !m_bApplyRootMotion)
		{
 			pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], nullptr, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);
			continue;
		}

		pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], pOwnerTransform, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);
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
	tMuDesc.fCurTrackPosition		= m_fCurrentTrackPosition;
	tMuDesc.iChannelCount			= m_iChannelCount;
	tMuDesc.iRootMotionBoneIndex	= m_iRootBoneIdx;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);
	
	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		if (iIndex == m_iRootBoneIdx && !m_bApplyRootMotion)
		{
			pChannel->SetUp_PoseData(spanLocalSrtData, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], nullptr, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);
			continue;
		}

		pChannel->SetUp_PoseData(spanLocalSrtData, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], pOwnerTransform, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);
	}
}

void CModelAnimation::Update_MixAnimation(const vector<class CBone*>& vecBones, CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS, const _float fTimeDelta, _uint iTotalBoneNum)
{
	Bind_AnimationMixData(pAnimMixCS, pPreAnimCS);

	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = 0.f;
	}

	// 가변 데이터 작성
	CS_MU_TRACK tMuDesc{};
	tMuDesc.fCurTrackPosition = m_fCurrentTrackPosition;
	tMuDesc.iChannelCount = m_iChannelCount;
	tMuDesc.iRootMotionBoneIndex = m_iRootBoneIdx;
	pAnimMixCS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimMixCS->Dispatch(iGroupX, 1, 1);


	// 원래 하던대로 channel update
	// todo_eunbi : mix는 root motion 안 건들인다는 마인드.. but 문제 생기면 다시 합시다
	_uint iIndex = { 0 };
	for (auto& pChannel : m_vecChannels)
	{
		_uint iBondIdx = pChannel->Get_BoneIndex();
		if (m_vecMixRatios[(size_t)iBondIdx] != 0.f)
		{
			if (iIndex == m_iRootBoneIdx && !m_bApplyRootMotion)
			{
				pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], nullptr, nullptr, fTimeDelta, m_fRootMotionOffset);
				continue;
			}

			pChannel->Update_TransformationMatrix(vecBones, m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[iIndex++], nullptr, nullptr, fTimeDelta, m_fRootMotionOffset);
		}

		else
			iIndex++;
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
	tMuDesc.iRootMotionBoneIndex = m_iRootBoneIdx;
	pAnimECS->Bind_Compute_Track(tMuDesc);
	
	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);

	//m_iRootChannelIdx
	if(m_bApplyRootMotion)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);

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
	tMuDesc.iRootMotionBoneIndex = m_iRootBoneIdx;
	pAnimECS->Bind_Compute_Track(tMuDesc);

	// dispatch
	_uint iGroupX = (iTotalBoneNum + 31) / 32;
	pAnimECS->Dispatch(iGroupX, 1, 1);

	//m_iRootChannelIdx
	if (m_bApplyRootMotion)
		m_vecChannels[(size_t)m_iRootChannelIdx]->Move_OnwerTransform(m_fCurrentTrackPosition, &m_vecCurrentKeyFrameIndices[(size_t)m_iRootChannelIdx], pOwnerTransform, pOwnerPhyCCT, fTimeDelta, m_fRootMotionOffset);

	//else
	//	int test = 0;
}

void CModelAnimation::Bind_AnimationEData(CComputeShader* pAnimEShader)
{
	pAnimEShader->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_KEYFRAME), m_pInputKeySB_SRV, m_pKeyFrameBuffer);
	pAnimEShader->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_CHANNELDATA), m_pInputChannelSB_SRV, m_pChannelDataBuffer);
}

void CModelAnimation::Bind_AnimationMixData(CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS)
{
	// animation 결과 blendCS에 bind
	pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRT),
		pAnimMixCS->Get_SRV("MU_PRETRANSFORMS"), pPreAnimCS->Get_Output_Buffer());

	auto pKeySRV = pAnimMixCS->Get_SRV("IMMU_KEYFRAMS");
	auto pChannelSRV = pAnimMixCS->Get_SRV("IMMU_CHANNELDATAS");

	//Bind_AnimationEData(pAnimMixCS);
	pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_KEYFRAME), pKeySRV, m_pKeyFrameBuffer);
	pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_CHANNELDATA), pChannelSRV, m_pChannelDataBuffer);
	pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::IMMU_MIXDATA), m_pMixSB_SRV, m_pMixDataBuffer);



	//// animation 결과 MixCS에 bind
	//pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRT),
	//	pAnimMixCS->Get_SRV("MU_PRETRANSFORMS"), pPreAnimCS->Get_Output_Buffer());
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
		pIniailChannelData[i].Padding0 = 0.f;

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

void CModelAnimation::Reset_PrePosition()
{
	// root bone이 있을때 root channel만 reset 해줌
	if (m_iRootBoneIdx >= 0)
	{
		m_vecChannels[m_iRootChannelIdx]->Reset_PreTranslation();
	}
}

void CModelAnimation::Set_MotionBone(_int iBondIdx)
{
	m_iRootBoneIdx = iBondIdx;

	for (size_t i = 0 ; i< m_vecChannels.size() ; i++)
	{
		if (m_vecChannels[i]->Set_MotionBone(iBondIdx))
		{
			m_iRootChannelIdx = (_uint)i;
		}
	}
}

void CModelAnimation::Set_MixRatio(vector<_float>& vecMixRatio, CComputeShader* pAnimMixCS)
{
	m_vecMixRatios = vecMixRatio;
	_uint iSize = _uint(m_vecMixRatios.size());

	if (m_pMixDataBuffer)
		Safe_Release(m_pMixDataBuffer);
	if (m_pMixSB_SRV)
		Safe_Release(m_pMixSB_SRV);

	// 3. struct buffer class 생성
	m_pMixDataBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_IMMU_ANIMMIX), iSize);

	// buffer의 내용을 채움
	CS_IMMU_ANIMMIX* pInitialData = new CS_IMMU_ANIMMIX[iSize];
	for (size_t  i = 0 ; i< iSize ; i++)
	{
		pInitialData[i].fMixRatio = m_vecMixRatios[i]; //m_vecMixRatios[i];
		pInitialData[i].Padding0 = Vec3::Zero;
	}

	// 4. buffer에 값 넣어줌
	m_pMixDataBuffer->Copy_Data(pInitialData, sizeof(CS_IMMU_ANIMMIX), iSize);

	// 5. 동적배열 정리
	Safe_Delete_Array(pInitialData);

	if (m_pMixDataBuffer == nullptr)
		return;

	// 4. SRV 연결
	m_pMixSB_SRV = pAnimMixCS->Get_SRV("IMMU_MIXDATA");
	m_pMixSB_SRV->SetResource(m_pMixDataBuffer->Get_SRV());

/*	m_pInputKeySB_SRV = pAnimMixCS->Get_SRV("IMMU_KEYFRAMS");
	m_pInputKeySB_SRV->SetResource(m_pKeyFrameBuffer->Get_SRV());

	m_pInputChannelSB_SRV = pAnimMixCS->Get_SRV("IMMU_CHANNELDATAS");
	m_pInputChannelSB_SRV->SetResource(m_pChannelDataBuffer->Get_SRV())*/;


	if (m_pMixSB_SRV)
		return;
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
	Safe_Release(m_pMixDataBuffer);

	if (!IsClone())
	{
		Safe_Release(m_pInputKeySB_SRV);
		Safe_Release(m_pInputChannelSB_SRV);

		//if (m_pMixDataBuffer)
		{
			Safe_Release(m_pMixSB_SRV);

		}
	}

	Super::Free();
}
