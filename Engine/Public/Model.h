#pragma once
#include "Component.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)

class CShader;
class CTransform;
class CPhysicsCCT;
class CComputeShader;
class StructuredBuffer;

class ENGINE_DLL CModel final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::MODEL;

	typedef struct tagMIXDATA
	{
		_uint	iParentIdx; // 이거의 자식들은 ani2를 섞을거다
		_bool	bInClude;	// 나도 포함 할거니
		_float	fRatio;		// 섞을 정도
	}DATA_ANIMIX;

	// test eunbi : animation 및 channel이 추가로 가져야 하는 정보들
	typedef struct tagAnimationData
	{
		_bool			bRootAni = { false };
		_bool			bMixAni = { false };
		
		_int			iRootBoneIndex = -1;
		vector<_float>	vecMixRatios;
	}DATA_ANIMCHANNEL;

	enum STAGEING_BONE : Flags
	{
		SB_ZEROBONE				= 0x001
		, SB_ALLBONE			= 0x002
		, SB_SPCIPICBONE		= 0x004
		, SB_STAGEING_FOR_INT	= 0x005
		, SB_STAGEING_FOR_NAME	= 0x006
	};

	typedef struct tagModelOriginDesc
	{
		EModelType			eType					= { EModelType::END };
		_int				iPrototypeLevelIndex	= { -1 };
		Matrix*				pMatPreTransform		= { nullptr };
		wstring				wstrModelFolderName		= { L"" };

		DATA_ANIMCHANNEL*	pAniChannelData			= { nullptr };

		Flags				FStageBone				= { STAGEING_BONE::SB_ZEROBONE }; // STAGEING_BONE flag 이용하시면 됩니다
		vector<_uint>		vecStageBoneIndices;	// 저장할 bone의 인덱스들. 만약 전체를 저장하고 싶다면 flag만 잘 설정하면 됨
		vector<string>		vecStageBoneName;


	}MODEL_ORIGIN_DESC;
	typedef struct tagModelCopyDesc
	{
		std::span<const EMaterialInstanceType>	spanMIs;
		std::span<const _int>					spanShaderPassesByMesh;
	}MODEL_COPY_DESC;

	struct BONE_GROUP
	{
		vector<_uint>							BoneIndices;
		StructuredBuffer*						pIndexBuffer		= { nullptr };
		ID3DX11EffectShaderResourceVariable*	pInputGroupSB_SRV	= { nullptr };
	};
	
	typedef struct tagGhostTrailDesc
	{
		_float	fInterval = { 0.15f };
		_float	fLifeTime = { 0.4f };
		_uint	iMaxCount = { 5 };
		Vec4	vColor = { 0.3f, 0.5f, 1.f, 0.7f };
	}GHOST_TRAIL_DESC;

	typedef struct tagGhostSnapshot
	{
		StructuredBuffer* pBoneBuffer = { nullptr };
		Matrix			  matWorld = { Matrix::Identity };
		_float			  fElapsed = { 0.f };
		_float			  fLifeTime = { 0.3f };
	}GHOST_SNAPSHOT;
private:
	enum AnimationPlayState
	{
		PLAY,
		BLEND,
		END
	};

	enum class AnimUpdateState { NORMAL, BLEND, MIX, ADDITIVE, RAGDOLL, MYMOVE };

	enum class CS_SB_IDX : _uint
	{
		IMMU_BONE, MU_GROUPIDX, MU_SRTS
	};

	enum class BLENDCS_SB_IDX : _uint
	{
		MU_PRESRT, MU_CURSRT
	};

	enum class GETBONECS_SB_IDX : _uint
	{
		IMMU_BONEINDICES, MU_BONEMATS
	};

	enum class CS_PARTBONE_IDX : _uint
	{
		IMMU_BONE, MU_PARENTTRANSFORM
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

	virtual HRESULT						Initialize_Prototype(void* pArg);
	virtual HRESULT						Initialize(void* pArg) override;

	// render funcs
public:
	HRESULT								Render(_uint iMeshIndex);
	HRESULT								Render_Instance(_uint iMeshIndex , _uint iInstanceCount);

	// animation funcs
public:
	HRESULT								Change_Animation(CComputeShader* pAnimEComShader, const wstring& wstrName, _bool bBlend, _bool isLoop = true, _bool bForce = false);
	HRESULT								Change_Animation(CComputeShader* pAnimEComShader,_uint iAnimationIndex, _bool bBlend, _bool isLoop = true, _bool bForce = false);
	void								Add_Animation(class CModelAnimation* pAnimation) { m_vecAnimations.push_back(pAnimation); }
	// Transform과 CCT를 바인딩 안할 시 RootMotion적용은 되나, 포지션을 반영안한다.
	void								Update_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEComShader, _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimBlendCS = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditiveCS = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr); // transform, phsics는 rootmotion 적용시 넘겨줘야함
	void								Update_PartModel(CComputeShader* pParentBoneComBineCS, CComputeShader* pChildBonePartCS);

	// ghots trail funcs
public:
	_bool Is_ActiveGhostTrail() const { return m_bGhostActive; }
	_bool Has_GhostTrailSnapshots() const;
	void Set_GhostTrailDesc(const GHOST_TRAIL_DESC& desc);
	void Enable_GhostTrail();
	void Disable_GhostTrail();
	void Clear_GhostTrail();
	void Set_GhostColor(const Vec4& vColor);
	void Update_GhostTrail(_float fTimeDelta);
	void Capture_Ghsot(CComputeShader* pBoneCombineCS, const Matrix& matWorld);
	HRESULT Render_GhostTrail(CShader* pShader, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS, _uint iGhostPass);
	// bind funcs
public:
	HRESULT								Bind_Material(class CShader* pShader, _uint iMeshIndex);
	HRESULT								Bind_MaterialInstance(class CShader* pShader, _uint iMeshIndex);
	HRESULT								Bind_Bones(class CShader* pShader, _uint iMeshIndex, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS, _uint iIndexDistance = 0);

	// tool funcs
public:
	void								Set_RootBone(_int iRootIdx);
	void								Set_Animtion_MotionOffset(_uint iAnimIdx, _float fOffset);
	_int								Get_RootBone() const { return m_iRootBoneIdx; }
	_float								Get_Animatioin_MotionOffset(_uint iAnimIdx);
	void								Set_Animation_Speed(_uint iAnimIdx, _float fSpeed);

	_bool								Get_Is_AdditiveOn() const { return m_bAdditiveAnim; }
	_int								Get_RefAdditive_AnimIdx() const { return m_iAdditivRef_AnimIdx; }
	_int								Get_PosAdditive_AnimIdx() const { return m_iAdditivePos_AnimIdx; }
	// mix anim funcs
public:
	void	Make_MixRatio(_uint iAnimIdx, vector<DATA_ANIMIX>& vecAniMixData, CComputeShader* pAnimMixCS);
	void	Set_MixAnim_ResetSize(_uint iSize);
	void	Set_MixAnim_AnimIndex(_uint iVectorIdx, _int iAnimIdx);
	void	Set_MixAnim(_bool bMix);

	void	Set_AdditiveRef_AnimIdx(_int  iAnimIdx) { m_iAdditivRef_AnimIdx = iAnimIdx; }
	void	Set_AdditivePos_AnimIdx(_int  iAnimIdx) { m_iAdditivePos_AnimIdx = iAnimIdx; }
	void	Set_Apply_AdditiveAnim(_bool  bAdditive) { m_bAdditiveAnim = bAdditive; }

	void	Set_AdditiveData(_bool bAdditive, _int iRefIdx, _int iPosIdx, _float fMixOffset = 1.f); // ref 까지 같이
	void	Set_AdditiveData(_bool bAdditive, _int iPosIdx, _float fMixOffset = 1.f); // ref 없이

	// tool
	_bool	Get_MixBool() const { return m_bMixAnim; }
	const vector<_int>& Get_MixIdx() const { return m_vecMixAnimIndices; }
	_uint Get_MixSize() const { return (_uint)m_vecMixAnimIndices.size(); }

public:
	void Set_MoveBone(_bool bMove) { m_bMoveBone = bMove; }
	void Set_MoveBoneCS(CS_CB_MU_BONEMOVE tCBData) { m_tBoneMoveCB = tCBData; m_tBoneMoveCB.iBoneNums = Get_BoneCount(); }
	void Set_MoveBone_Ratio(_float fRatio) { m_tBoneMoveCB.fRatio = fRatio; }
	void SEt_MoveBone_Matrix(const Matrix& matOffset) { m_tBoneMoveCB.matOffset = matOffset; }

	_int Get_BoneIndex(const string& strName);
	_bool Get_MoveBoneOn() const { return m_bMoveBone; }
	const CS_CB_MU_BONEMOVE& Get_MoveInfo() const { return m_tBoneMoveCB; }

	// getter funcs
public:
	EModelType							Get_Type() const { return m_eType; }

	// materials
	const vector<class CMaterial*>&		Get_Materials() const { return m_vecMaterials; };
	const Vec3*							Get_StaticModelMinMax() const { return m_pStaticModel_MinMax; };
	wstring								Get_MaterialName(_uint iIndex) const;

	// meshes
	_int								Get_PassByMesh(_uint iMeshIndex);
	// counts
	_uint								Get_AnimationCount()	const { return static_cast<_uint>(m_vecAnimations.size()); }
	_uint								Get_MaterialCount()		const { return static_cast<_uint>(m_vecMaterials.size()); }
	_uint								Get_BoneCount()			const { return static_cast<_uint>(m_vecBones.size()); }
	_uint								Get_MeshCount()			const { return static_cast<_uint>(m_vecMeshes.size()); }
	_uint								Get_StageBoneCount()	const { return m_iStageBoneCounts; }

	//m_iStageBoneCounts

	_int								Get_BoneIndex(const _char* szName);
	_int								Get_CurrentAnimationIndex() const;
	_int								Get_AnimationIndex(const wstring& wstrName);

	// get class pointer
	class CMesh*						Get_Mesh(_uint iIndex);
	class CBone*						Get_Bone(const _char* szName);
	class CBone*						Get_Bone(_uint iIndex);
	class CMaterialInstance*			Get_MaterialInstance(_uint iIndex);
	class CModelAnimation*				Get_Animation(_uint iIndex);
	class CModelAnimation*				Get_CurrentAnimation();

	// animations
	_bool								Is_AnimFinished() const { return m_bIsAnimFinished; }
	_bool								Is_AnimTrackPositionBetween(_float fStartRatio, _float fEndRatio);
	_bool								Is_AnimTrackPositionBetweenRaw(_float fTrackA, _float fTrackB);
	_bool								Is_AnimTrackPositionAt(_float fRatio);
	_bool								Is_AnimTrackPositionAtRaw(_float fTrackPosition);
	_bool								Is_AnimTrackPositionAtHalf() const;

	_bool								Is_LoopAnimDone() const { return m_bLoopAnimDone; }
	_bool								Is_RootMotion_Apply() const;


	_float								Get_AnimDurationTime() const;
	_float								Get_AnimNormalizedTime() const;
	_float								Get_AnimElpasedTimeSeconds() const;

	_float								Get_AnimTickPerSecond() const;
	void								Set_AnimTickPerSecond(_float fValue);

	wstring								Get_CurrentAnimationName() const;
	wstring								Get_AnimationName(_uint iIdex) const;

	const _float						Get_BlentTime() const { return m_fBlendedTime; }
	_float								Get_AnimTrackPosition() const;

	// setter funcs
public:
	HRESULT								Set_DefaultMI(_uint iIndex);
	HRESULT								Set_PassByMesh(class CShader* pShader, _uint iMeshIndex);
	HRESULT								Set_DefaultPassByMesh(_uint iMeshIndex);
	void								Set_AnimationPlayRate(_uint iIndex, _float fValue);
	void								Set_AnimationSpeed(_float fSpeed) { m_fAnimationSpeed = fSpeed; if (m_fAnimationSpeed <= 0) m_fAnimationSpeed = 1.f; }
	void								Set_CurAnimation_RootApply(_bool bRootApply);
	void								Set_ApplyRootMotionAll(_bool bRootApply);

	void								Set_Animtion_MotionOffset_All(_float fOffset);
	void								Set_Animation_SpeedOffset_All(_float fOffset);

	void								Set_ApplyRagDoll(_bool bApply) { m_bRagDollOn = bApply; }

public:
	HRESULT								Set_MI(_uint iIndex, const SHADER_MI_DESC& tDesc);
	HRESULT								Set_MI_TintColor(_uint iIndex, const Vec4& vColor );
	HRESULT								Set_MI_EmissiveColor(_uint iIndex, const Vec4& vColor );
	HRESULT								Set_MI_EmissivePower(_uint iIndex, const float& fPower );
public:
	HRESULT								Change_MI(_uint iIndex, EMaterialInstanceType eChangeType );
	HRESULT								Change_Material(_uint iIndex, const wstring& wstrMaterialTag);
	HRESULT								Change_ShaderPassByMseh(_uint iMeshIndex, _uint iPass);

public:
	HRESULT								Ready_ComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditiveCS = nullptr);
	HRESULT								Ready_PartComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBonePartCS, CModel* pParentModel);
	void								Get_BoneMatrix(CComputeShader* pAnimMixCS);

	// load func

	// for animation tool
	AnimationPlayState					Get_AnimPlayState() { return m_eCurrentAnimationState; }
	vector<class CBone*>&				Get_Bones() { return m_vecBones; }
	vector<class CModelAnimation*>&		Get_Animations() { return m_vecAnimations; }
	_bool								Is_Loop() { return m_isAnimLoop; }
	void								Set_LoopState(_bool bValue) { m_isAnimLoop = bValue; }
	void								Set_AnimTrackPosition(_float fValue);

private:
	HRESULT								Load_StaticModel(const wstring& wstrModelName);
	HRESULT								Load_NonAnimModel(const wstring &wstrModelName);
	HRESULT								Load_AnimModel(const wstring& wstrModelName, DATA_ANIMCHANNEL* pData = nullptr);
	HRESULT								Load_CustomPartsModel(const wstring& wstrModelName);
	HRESULT								Load_OnlyBone(const wstring& wstrModelName);
	HRESULT								Add_Parts(const wstring &wstrPrototypeTag);
	CModel*								Get_Clone(const wstring &wstrPrototypeTag);


	// animation funcs
private:
	void								Play_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditive = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr);
	void								Blend_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, _float fRatio, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditive = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr);

	HRESULT								Build_AnimationIndexTable();
	void								Begin_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0, _bool bChannelReset = true);
	void								Update_AnimationPlayState(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditive = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr);
	void								End_AnimationPlayState(AnimationPlayState eState, AnimationPlayState eNextState);
	void								Change_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0, _bool bChannelReset = true);

	void								Play_Begin(CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0, _bool bChannelReset = true);
	void								Play_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditive = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr);
	void								Play_End(AnimationPlayState eNextState);

	void								Blend_Begin(_uint CurAnimationIndex);
	void								Blend_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimMixCS = nullptr, CComputeShader* pAdditive = nullptr, CComputeShader* pRagDollCS = nullptr, CComputeShader* pBoneMoveCS = nullptr);
	void								Blend_End();

	// ready funcs
private:
	void								Make_BoneGroup();
	void								Make_GroupBuffers();
	void								Make_SB();
	void								Make_Staging(MODEL_ORIGIN_DESC* pDesc);
	HRESULT								Ready_StaticModelMinMax();

	void								Set_CpuBone(_uint iBoneIdx);

	// cs bind funcs
private:
	void								Bind_BoneImmuData(CComputeShader* pBoneComBineCS);
	void								Bind_BufferSRV(CComputeShader* pBoneComBineCS);
	void								Ready_SB(CComputeShader* pAnimEvalCS);
	HRESULT								Bind_StagingBuffer(CComputeShader* pAnimMixCS);

	// cs update funcs
private:
	void								Update_BoneCombineTransformMatrix(CComputeShader* pBoneComBineCS);
	void								Lerp_Animation(CComputeShader* pAnimBlendCS, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);
	void								DisPatch_BondMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pAnimMixCS);
	void								Mix_Animation(CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS, const _float fTimeDelta);
	_bool								Additive_Animation(CComputeShader* pAdditiveCS, CComputeShader* pPreAnimCS, const _float fTimeDelta ,CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT);

	///////////////
	//// Event ////
	///////////////
public:
	// 외부 공개용, 무조건 현재프레임의 애니메이션 기준의 정보
	HRESULT								Emit_Notifies(EAnimNotifyPhase ePhase);
private:
	void								Emit_Notifies(CModelAnimation* pAnimation, _float fCurPos, EAnimNotifyPhase ePhase);

	/////////////////
	////  Ragdoll  //
	/////////////////
public:
	array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> Get_RagdollBoneDesc() { return m_arrRagdollBoneDesc; }

private:
	void								Mapping_Ragdoll_Bone();
	RAGDOLLBONEDESC						Set_Ragdoll_Bone(RAGDOLLJOINT::Enum eJoint, RAGDOLLJOINT::Enum eParentJoint, RAGDOLLJOINT::Enum eChildJoint);

private:
	CModel::GHOST_TRAIL_DESC			Init_GhostTrailDesc(CModel::GHOST_TRAIL_DESC tDesc);

private:
	EModelType							m_eType						= { EModelType::END };
	Matrix								m_matPreTransform			= {};

	ID3D11Device*						m_pDevice					= { nullptr };
	ID3D11DeviceContext*				m_pDeviceContext			= { nullptr };

	/* Model Minmax */
	Vec3*								m_pStaticModel_MinMax		= { nullptr };

	vector<_uint>						m_vecPasses;
	vector<class CBone*>				m_vecBones;
	vector<class CMesh*>				m_vecMeshes;
	vector<class CMaterial*>			m_vecMaterials;
	vector<class CMaterialInstance*>	m_vecMaterialInstances;

	/* animation */
	AnimationPlayState					m_eCurrentAnimationState	= { AnimationPlayState::PLAY };
	_bool								m_isAnimLoop				= { false };
	_bool								m_bIsAnimFinished			= { false };
	_float								m_fBlendedTime				= { 0.f };
	const _float						m_fBlendDuration			= { 0.25f };
	_uint								m_iAnimationCount			= { 0 };
	_uint								m_iCurrentAnimIndex			= { 0 };
	_uint								m_iPrevAnimIndex			= { 0 };
	unordered_map<wstring, size_t>		m_umapAnimationIndexTable;
	vector<class CModelAnimation*>		m_vecAnimations;
	vector<LOCALSRT>					m_vecPrevAnimationPose;
	vector<LOCALSRT>					m_vecCurrAnimationPose;

	/* ghost trail */
	_bool								m_bGhostActive				= { false };
	_uint								m_iGhostSpawnedCount		= { 0 };
	GHOST_TRAIL_DESC					m_tGhostTrail				= {};
	_float								m_fGhostTrailDelta			= { 0.f };
	_float								m_fGhostAccTime				= { 0.f };
	vector<GHOST_SNAPSHOT>				m_vecGhostSnapshots;
private:
	_int								m_iRootBoneIdx				= { -1 };
	Vec3								m_vPreMainPosition			= { Vec3::Zero };
	Vec3								m_vPreBlendPosition			= { Vec3::Zero };
	Vec3								m_vPreMixPosition			= { Vec3::Zero };
	Vec3								m_vPrePosNon				= { Vec3::Zero };

	Quat								m_vPreQuat					= { Quat::Identity };
	Quat								m_vPreBlendQuat				= { Quat::Identity };
	Quat								m_vPreQuatMix				= { Quat::Identity };
	Quat								m_vPreQuatNon				= { Quat::Identity };

	// mix anim
	vector<_int>						m_vecMixAnimIndices;
	_bool								m_bMixAnim = { false };

	// additive anim
	_int								m_iAdditivRef_AnimIdx	= { -1 };
	_int								m_iAdditivePos_AnimIdx	= { -1 };
	_float								m_fAdditiveOffset		= { 1.f };
	_bool								m_bAdditiveAnim			= { false };

	_uint								m_iBlendRootType = { 0 };

	// move bone
	_bool								m_bMoveBone = { false };
	CS_CB_MU_BONEMOVE					m_tBoneMoveCB = {};

	// compute shader 변수
private:
	vector<BONE_GROUP>					m_vecBoneGroups;
	_bool								m_bStageBones				= { false };
	_uint								m_iStageBoneCounts			= { 0 };
	vector<_uint>						m_vecStageBoneIndices;

	StructuredBuffer*					m_pPreSB					= { nullptr };
	StructuredBuffer*					m_pCurSB					= { nullptr };
	//ID3D11Buffer*						m_pBoneOuputStagingBuffer[2] = {nullptr};
	_uint								m_iFrameIndex = { 0 };

	_uint m_iCpuBoneCount = { 0 };
	_bool m_bLoopAnimDone = { false };
	_float m_fAnimationSpeed = { 1.f };

	_bool m_bRagDollOn = { false };

	AnimUpdateState m_eAnim_UpdateState = { AnimUpdateState::NORMAL };

	///////////////
	//// Event ////
	///////////////
	_bool m_bLooped{ false }; // 외부 공개용 아님, 현재프레임에 Loop가 이루어졌는가?
	_float m_fAnimPrevTrackPosition{ -1.f }; // 외부 공개용 아님
public:
	CMulticastDelegate<void(const AnimNotifyKey&)> OnNotify;

	array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> m_arrRagdollBoneDesc;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END