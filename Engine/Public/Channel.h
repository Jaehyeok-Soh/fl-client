#pragma once
#include "Base.h"

/*
* Channel : 이 뼈(Channel)의 어떤 시간에 어떤 상태를 취한다는 것들을 저장히기 위한것
* KeyFarme : 뼈의 상태 단위
* 상태와 상태 사이를 보간하여 상태를 만들어준다.
* 만들어 낸 상태는 CBone의 m_Transformationmatrix에 전달한다.
* 왜? 애니메이션 데이터는 뼈의 순수 Local의 데이터
*/

NS_BEGIN(Engine)

class CTransform;
class CPhysicsCCT;

class CChannel final : public CBase
{
	using Super = CBase;
public:
	typedef struct tagChannelDesc
	{
		_int iBoneIndex = { -1 };
		string strName = { "" };
		std::span<KEYFRAME> spanKeyframes;

		_int	iRootBoneIndex = {-1};
	}CHANNEL_DESC;
private:
	CChannel();
	virtual ~CChannel() = default;

	HRESULT Initialize(const CHANNEL_DESC& desc);
public:
	// normal anim
	void Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);
	void SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

	// motion anim
	void Update_TransformationMatrix(const vector<class CBone*>& vecBones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);
	void SetUp_PoseData(std::span<LOCALSRT> spanLocalSrtData, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);

	// 기본 channel 정보
private:
	_char				m_szName[MAX_NAME];
	_int				m_iBoneIndex	 = { -1 };
	_uint				m_iKeyFrameCount = { 0 };
	vector<KEYFRAME>	m_vecKeyframes;

private:
	_bool				m_bRootBone		= { false };
	Matrix				m_matTrans = {};
	Vec3				m_vPreRootLocal = Vec3::Zero;

private:
	void Update_MotionBone(Vec3 vLeftTrans, Vec3 vRightTrans , CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);

public:
	static CChannel* Create(const CHANNEL_DESC &desc);
	virtual void Free() override;
};

NS_END