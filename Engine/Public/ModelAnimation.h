#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

// 특정 동작을 위한 뼈대들의 시간에 따른 상태값(행렬)을 보관한다.
// CChannel 특정 뼈의 시간에 따른 상태값을 보관

class CTransform;
class CPhysicsCCT;

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
	}MODELANIM_DESC;
private:
	CModelAnimation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModelAnimation(const CModelAnimation& rhs);
	virtual ~CModelAnimation() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	_bool Update_TransformationMatrices(const vector<class CBone*>& vecBones, _float fTimeDelta, _bool isLoop, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);
	void SetUp_PoseDatasForBlending(std::span<LOCALSRT> spanLocalSrtData, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);
	void Clear();

	_float Get_DurationTime() const { return m_fDuration; }
	_float Get_NormalizedTime() const { return m_fCurrentTrackPosition / m_fDuration; }
	_float Get_ElpasedTimeSeconds() const { return m_fCurrentTrackPosition / m_fTickPerSecond; }
	void Set_PlayRate(_float fRate) { m_fTickPerSecond *= fRate; }
	_bool Is_TrackPositionBetween(_float fStartRatio, _float fEndRatio);
	_bool Is_TrackPositionAt(_float fRatio) const { return m_fCurrentTrackPosition >= m_fDuration * fRatio; }
	_bool Is_TrackPositionAtHalf() const { return Is_TrackPositionAt(0.5f); }
private:
	_uint m_iChannelCount = { 0 };
	vector<class CChannel*> m_vecChannels;
	vector<_uint> m_vecCurrentKeyFrameIndices;

	_float m_fCurrentTrackPosition = { 0.f }; // 현재 애니메이션 위치
	_float m_fTickPerSecond = { 0.f }; // 애니메이션 재생 속도
	_float m_fDuration = { 0.f }; // 현재 애니메이션의 전체 재생 길이
public:
	static CModelAnimation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	CModelAnimation* Clone();
	virtual void Free() override;
};

NS_END