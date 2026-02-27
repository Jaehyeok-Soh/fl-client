#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

// 특정 동작을 위한 뼈대들의 시간에 따른 상태값(행렬)을 보관한다.
// CChannel 특정 뼈의 시간에 따른 상태값을 보관

class CTransform;
class CPhysicsCCT;
class CComputeShader;
class StructuredBuffer;

class CModelAnimation final : public CResourceBase
{
	using Super = CResourceBase;
public:
	typedef struct tagModelAnimationDesc : public Super::RESOURCE_BASE_DESC
	{
		_uint iChannelCount = { 0 };
		_float fDuration = { 0.f };
		_float fTickPerSecond = { 0.f };
		std::span<class CChannel*> spanChannels;

		_bool bRootAni		= {false};
		_bool bMixAni		= {false};
		_int iRootBondIndex = { -1 };
	}MODELANIM_DESC;

private:
	enum class CS_SB_IDX : _uint
	{
		IMMU_KEYFRAME
		, IMMU_CHANNELDATA
		, IMMU_MIXDATA
		, MU_SRT
	};
private:
	CModelAnimation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModelAnimation(const CModelAnimation& rhs);
	virtual ~CModelAnimation() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	_bool Update_TransformationMatrices(const vector<class CBone*>& vecBones,_bool& bLoopDone, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimECS);
	void SetUp_PoseDatasForBlending(std::span<LOCALSRT> spanLocalSrtData, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, _uint iTotalBoneNum, CComputeShader* pAnimECS);
	void	Update_MixAnimation(const vector<class CBone*>& vecBones, CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS, const _float fTimeDelta, _uint iTotalBoneNum, _bool bFirst);
	void Clear();

	_float Get_DurationTime() const { return m_fDuration; }
	_float Get_NormalizedTime() const { return m_fCurrentTrackPosition / m_fDuration; }
	_float Get_ElpasedTimeSeconds() const { return m_fCurrentTrackPosition / m_fTickPerSecond; }
	void Set_PlayRate(_float fRate) { m_fTickPerSecond *= fRate; }
	_float Get_TrackPosition() { return m_fCurrentTrackPosition; }
	void Set_TrackPosition(_float fValue) { m_fCurrentTrackPosition = fValue; }
	_bool Is_TrackPositionBetween(_float fStartRatio, _float fEndRatio);
	_bool Is_TrackPositionAt(_float fRatio) const { return m_fCurrentTrackPosition >= m_fDuration * fRatio; }
	_bool Is_TrackPositionAtHalf() const { return Is_TrackPositionAt(0.5f); }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }
	void Set_TickPerSecond(_float fValue) { m_fTickPerSecond = fValue; }

	void	Bind_AnimationEData(CComputeShader* pAnimEShader);
	void	Bind_AnimationMixData(CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS);
	HRESULT Ready_BindBuffers(CComputeShader* pAnimESahder);

	// test
	void Check_UpdateCpu(const vector<class CBone*>& vecBones);
	void Reset_PrePosition();

public:
	void Set_MotionBone(_int iBondIdx);
	void Set_MotionOffset(_float fOffset) { m_fRootMotionOffset = fOffset; if (m_fRootMotionOffset < 0.f) m_fRootMotionOffset = 1.f; }
	_float Get_MotionOffset() const { return m_fRootMotionOffset; }
	void Set_MixRatio(vector<_float>& vecMixRatio, CComputeShader* pAnimMixCS);
	void Set_ApplyRootMotion(_bool Apply) { 
		m_bApplyRootMotion = Apply; 
	};
	_bool Get_ApplyRoot() const { return m_bApplyRootMotion; }
	void Set_AnimationSpeed(_float fSpeed) { m_fAnimationSpeed_Offset = fSpeed; if (m_fAnimationSpeed_Offset <= 0.f) m_fAnimationSpeed_Offset = 1.f; }

	///////////////
	//// Event ////
	///////////////
	const vector<AnimNotifyKey>& Get_Notifies(EAnimNotifyPhase ePhase) const { return m_vecNotifies[ENUM_TO_UINT(ePhase)]; }
	void Set_Notifies(EAnimNotifyPhase ePhase, vector<AnimNotifyKey> vecKeys);
	void Pushback_Notifies(EAnimNotifyPhase ePhase, const AnimNotifyKey& key);
	void Sort_Notifies();
	void Reset_NotifyCursor()
	{
		for (auto& iCursor : m_iNextNotifyIndices)
			iCursor = 0;
	}
	_uint Get_NotifyCursor(EAnimNotifyPhase ePhase) const { return m_iNextNotifyIndices[ENUM_TO_UINT(ePhase)]; }
	void Set_NotifyCursor(EAnimNotifyPhase ePhase, size_t iIndex)
	{
		if (m_vecNotifies[ENUM_TO_UINT(ePhase)].size() <= iIndex) return;
		m_iNextNotifyIndices[ENUM_TO_UINT(ePhase)] = (_uint)iIndex;
	}
	void Clear_Notifies() { for (auto& notifies : m_vecNotifies) notifies.clear(); }
private:
	_uint m_iChannelCount = { 0 };
	vector<class CChannel*> m_vecChannels;
	vector<_uint> m_vecCurrentKeyFrameIndices;

	_float m_fCurrentTrackPosition = { 0.f };	// 현재 애니메이션 위치
	_float m_fTickPerSecond = { 0.f };			// 애니메이션 재생 속도
	_float m_fDuration = { 0.f };				// 현재 애니메이션의 전체 재생 길이

	/* compute shader */
private:
	_uint											m_iKeyFrameBufferSize	= {};
	StructuredBuffer*								m_pKeyFrameBuffer		= {nullptr};
	ID3DX11EffectShaderResourceVariable*			m_pInputKeySB_SRV		= { nullptr };

	_uint											m_iChannelSize			= {};
	StructuredBuffer*								m_pChannelDataBuffer	= {nullptr};
	ID3DX11EffectShaderResourceVariable*			m_pInputChannelSB_SRV	= { nullptr };

	vector<_float>									m_vecMixRatios;
	StructuredBuffer*								m_pMixDataBuffer = { nullptr };
	ID3DX11EffectShaderResourceVariable*			m_pMixSB_SRV = { nullptr };

private:
	_int m_iRootBoneIdx = { -1 };
	_int m_iRootChannelIdx = { -1 };

	_bool m_bApplyRootMotion		= { true };
	_float m_fRootMotionOffset		= { 1.f };
	_float m_fAnimationSpeed_Offset = { 1.f };

	///////////////
	//// Event ////
	///////////////
private:
	vector<AnimNotifyKey> m_vecNotifies[ENUM_TO_UINT(EAnimNotifyPhase::END)];
	_uint m_iNextNotifyIndices[ENUM_TO_UINT(EAnimNotifyPhase::END)]{ 0 };

private:
	HRESULT Ready_Buffers();
	
public:
	static CModelAnimation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	CModelAnimation* Clone();
	virtual void Free() override;
};

NS_END