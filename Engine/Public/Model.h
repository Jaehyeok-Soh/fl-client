#pragma once
#include "Component.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)

class CTransform;
class CPhysicsCCT;
class CComputeShader;
class StructuredBuffer;

class ENGINE_DLL CModel final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::MODEL;

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
		SB_ZEROBONE			= 0x001
		, SB_ALLBONE		= 0x002
		, SB_SPCIPICBONE	= 0x004
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

	}MODEL_ORIGIN_DESC;
	typedef struct tagModelCopyDesc
	{
		std::span<const EMaterialInstanceType> spanMIs;
		std::span<const _int> spanShaderPassesByMesh;
	}MODEL_COPY_DESC;

	struct BONE_GROUP
	{
		vector<_uint>							BoneIndices;
		StructuredBuffer*						pIndexBuffer		= { nullptr };
		ID3DX11EffectShaderResourceVariable*	pInputGroupSB_SRV	= { nullptr };
	};
	
private:
	enum AnimationPlayState
	{
		PLAY,
		BLEND,
		END
	};

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
	HRESULT								Change_Animation(CComputeShader* pAnimEComShader,_uint iAnimationIndex, _bool bBlend, _bool isLoop = true, _bool bForce = false);
	void								Add_Animation(class CModelAnimation* pAnimation) { m_vecAnimations.push_back(pAnimation); }
	void								Update_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEComShader, _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pAnimBlendCS = nullptr, CComputeShader* pGetBoneCS = nullptr); // transform, phsics는 rootmotion 적용시 넘겨줘야함

	// bind funcs
public:
	HRESULT								Bind_Material(class CShader* pShader, _uint iMeshIndex);
	HRESULT								Bind_MaterialInstance(class CShader* pShader, _uint iMeshIndex);
	HRESULT								Bind_Bones(class CShader* pShader, _uint iMeshIndex, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS, _uint iIndexDistance = 0);

	// tool funcs
public:
	void Set_RootBone(_int iRootIdx);

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

	// animations
	_bool								Is_AnimFinished() const { return m_bIsAnimFinished; }
	_bool								Is_AnimTrackPositionBetween(_float fStartRatio, _float fEndRatio);
	_bool								Is_AnimTrackPositionAt(_float fRatio);
	_bool								Is_AnimTrackPositionAtHalf() const;


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

	// materials funcs
public:
	HRESULT								Change_MI(_uint iIndex, const wstring& wstrMITag);
	HRESULT								Change_Material(_uint iIndex, const wstring& wstrMaterialTag);
	HRESULT								Change_ShaderPassByMseh(_uint iMeshIndex, _uint iPass);

public:
	HRESULT								Ready_ComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS = nullptr, CComputeShader* pGetBoneCS = nullptr);
	void								Get_BoneMatrix(CComputeShader* pGetBoneCS);

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
	void								Play_Animation(_float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	void								Blend_Animation(_float fTimeDelta, _float fRatio, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);

	void								Play_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pGetBoneCS = nullptr);
	void								Blend_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, _float fRatio, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pGetBoneCS = nullptr);

	HRESULT								Build_AnimationIndexTable();
	void								Begin_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0);
	void								Update_AnimationPlayState(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pGetBoneCS = nullptr);
	void								End_AnimationPlayState(AnimationPlayState eState);
	void								Change_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0);

	void								Play_Begin(CComputeShader* pAnimEvalCS = nullptr, _uint iAnimationIndex =0);
	void								Play_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pGetBoneCS = nullptr);
	void								Play_End();

	void								Blend_Begin();
	void								Blend_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr, CComputeShader* pGetBoneCS = nullptr);
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
	HRESULT								Bind_StagingBuffer(CComputeShader* pGetBoneCS);

	// cs update funcs
private:
	void								Update_BoneCombineTransformMatrix(CComputeShader* pBoneComBineCS);
	void								Lerp_Animation(CComputeShader* pAnimBlendCS, _float fRatio);
	void								DisPatch_BondMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pGetBoneCS);

	///////////////
	//// Event ////
	///////////////
private:
	void								Emit_Notifies(CModelAnimation* pAnimation, _float fPrevPos, _float fCurPos, _bool bIsLooped);

private:
	EModelType							m_eType						= { EModelType::END };
	Matrix								m_matPreTransform			= {};

	ID3D11Device*						m_pDevice					= { nullptr };
	ID3D11DeviceContext*				m_pDeviceContext			= { nullptr };

	/* Model Minmax */
	Vec3*								m_pStaticModel_MinMax		= {nullptr};

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
	const _float						m_fBlendDuration			= { 0.2f };
	_uint								m_iAnimationCount			= { 0 };
	_uint								m_iCurrentAnimIndex			= { 0 };
	_uint								m_iPrevAnimIndex			= { 0 };
	unordered_map<wstring, size_t>		m_umapAnimationIndexTable;
	vector<class CModelAnimation*>		m_vecAnimations;
	vector<LOCALSRT>					m_vecPrevAnimationPose;
	vector<LOCALSRT>					m_vecCurrAnimationPose;

private:
	_int								m_iRootBoneIdx				= { -1 };

	// compute shading 변수
private:
	vector<BONE_GROUP>					m_vecBoneGroups;
	_bool								m_bStageBones				= { false };
	_uint								m_iStageBoneCounts			= { 0 };
	vector<_uint>						m_vecStageBoneIndices;

	StructuredBuffer*					m_pPreSB					= { nullptr };
	StructuredBuffer*					m_pCurSB					= { nullptr };
	ID3D11Buffer*						m_pBoneOuputStagingBuffer[2] = {nullptr};
	_uint								m_iFrameIndex = { 0 };

	_uint m_iCpuBoneCount = { 0 };

	///////////////
	//// Event ////
	///////////////
public:
	CMulticastDelegate<void(const AnimNotifyKey&)> OnNotify;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END