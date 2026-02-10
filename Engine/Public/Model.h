#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CTransform;
class CPhysicsCCT;

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


	typedef struct tagModelOriginDesc
	{
		EModelType eType = { EModelType::END };
		_int iPrototypeLevelIndex = { -1 };
		Matrix* pMatPreTransform = { nullptr };
		wstring wstrModelFolderName = { L"" };

		DATA_ANIMCHANNEL* pAniChannelData = { nullptr };
	}MODEL_ORIGIN_DESC;
	typedef struct tagModelCopyDesc
	{
		std::span<const EMaterialInstanceType> spanMIs;
		std::span<const _int> spanShaderPassesByMesh;
	}MODEL_COPY_DESC;

private:
	enum AnimationPlayState
	{
		PLAY,
		BLEND,
		END
	};
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Render(_uint iMeshIndex);
	HRESULT	Render_Instance(_uint iMeshIndex , _uint iInstanceCount);
	HRESULT Change_Animation(_uint iAnimationIndex, _bool bBlend, _bool isLoop = true, _bool bForce = false);
	void	Add_Animation(class CModelAnimation* pAnimation) { m_vecAnimations.push_back(pAnimation); }
	void	Update_Animation(_float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	HRESULT Set_PassByMesh(class CShader* pShader, _uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex);
	HRESULT Bind_MaterialInstance(class CShader* pShader, _uint iMeshIndex);
	HRESULT Bind_Bones(class CShader* pShader, _uint iMeshIndex, _uint iIndexDistance = 0);
	HRESULT Bind_Masterbones(class CShader* pShader, _uint iIndexDistance);
public:
	HRESULT Change_ShaderPassByMseh(_uint iMeshIndex, _uint iPass);
	HRESULT Set_DefaultPassByMesh(_uint iMeshIndex);
	_int	Get_PassByMesh(_uint iMeshIndex);
	HRESULT Change_MI(_uint iIndex, const wstring& wstrMITag);
	HRESULT Set_DefaultMI(_uint iIndex);
	HRESULT Change_Material(_uint iIndex, const wstring& wstrMaterialTag);
	class CMaterialInstance* Get_MaterialInstance(_uint iIndex);
	EModelType Get_Type() const { return m_eType; }
	_uint Get_AnimationCount() const { return static_cast<_uint>(m_vecAnimations.size()); }
	_uint Get_MaterialCount() const { return static_cast<_uint>(m_vecMaterials.size()); }
	_uint Get_BoneCount() const { return static_cast<_uint>(m_vecBones.size()); }
	_uint Get_MeshCount() const { return static_cast<_uint>(m_vecMeshes.size()); }
	class CMesh* Get_Mesh(_uint iIndex);
	_int Get_BoneIndex(const _char* szName);
	class CBone* Get_Bone(const _char* szName);
	class CBone* Get_Bone(_uint iIndex);
	_float Get_AnimDurationTime() const;
	_float Get_AnimNormalizedTime() const;
	_float Get_AnimElpasedTimeSeconds() const;
	_int Get_CurrentAnimationIndex() const;
	wstring Get_CurrentAnimationName() const;
	_float Get_AnimTrackPosition() const;
	_bool Is_AnimFinished() const { return m_bIsAnimFinished; }
	_bool Is_AnimTrackPositionBetween(_float fStartRatio, _float fEndRatio);
	_bool Is_AnimTrackPositionAt(_float fRatio);
	_bool Is_AnimTrackPositionAtHalf() const;
	_int Get_AnimationIndex(const wstring& wstrName);
	wstring Get_MaterialName(_uint iIndex) const;
	const vector<class CMaterial*>&	Get_Materials() const { return m_vecMaterials; };
	wstring Get_AnimationName(_uint iIdex) const;
	void Set_AnimationPlayRate(_uint iIndex, _float fValue);
private:
	HRESULT Load_StaticModel(const wstring& wstrModelName);
	HRESULT Load_NonAnimModel(const wstring &wstrModelName);
	HRESULT Load_AnimModel(const wstring& wstrModelName, DATA_ANIMCHANNEL* pData = nullptr);
	HRESULT Load_CustomPartsModel(const wstring& wstrModelName);
	HRESULT Load_OnlyBone(const wstring& wstrModelName);
	HRESULT Add_Parts(const wstring &wstrPrototypeTag);
	CModel* Get_Clone(const wstring &wstrPrototypeTag);
	void	Play_Animation(_float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	void	Blend_Animation(_float fTimeDelta, _float fRatio, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);

private:
	HRESULT Build_AnimationIndexTable();
	void Begin_AnimationPlayState(AnimationPlayState eState);
	void Update_AnimationPlayState(const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	void End_AnimationPlayState(AnimationPlayState eState);
	void Change_AnimationPlayState(AnimationPlayState eState);

	void Play_Begin();
	void Play_Update(const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	void Play_End();

	void Blend_Begin();
	void Blend_Update(const _float fTimeDelta, CTransform* pOwnerTransform = nullptr, CPhysicsCCT* pOwnerPhyCCT = nullptr);
	void Blend_End();
private:
	EModelType m_eType = { EModelType::END };
	Matrix m_matPreTransform = {};

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	vector<_uint> m_vecPasses;
	vector<class CBone*> m_vecBones;
	vector<class CMesh*> m_vecMeshes;
	vector<class CMaterial*> m_vecMaterials;
	vector<class CMaterialInstance*> m_vecMaterialInstances;
	class CMesh* m_pMasterMesh = { nullptr };

	// Animation
	AnimationPlayState m_eCurrentAnimationState = { AnimationPlayState::PLAY };
	_bool m_isAnimLoop = { false };
	_bool m_bIsAnimFinished = { false };
	_float m_fBlendedTime = { 0.f };
	const _float m_fBlendDuration = { 0.18f };
	_uint m_iAnimationCount = { 0 };
	_uint m_iCurrentAnimIndex = { 0 };
	_uint m_iPrevAnimIndex = { 0 };
	unordered_map<wstring, size_t> m_umapAnimationIndexTable;
	vector<class CModelAnimation*> m_vecAnimations;
	vector<LOCALSRT> m_vecPrevAnimationPose;
	vector<LOCALSRT> m_vecCurrAnimationPose;
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END