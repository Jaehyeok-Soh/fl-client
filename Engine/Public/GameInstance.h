#pragma once
#include "Base.h"
#include <random>
#include "RenderTarget_Manager.h"
#include "Resource_Manager.h"
#include "EventBus_Manager.h"
#include "DataRepository.h"
#include "Anim_Event_Hitbox.h"
#include "PhysicsAttackRaycast.h"

NS_BEGIN(Engine)
 
#pragma region MACRO
#define KEY_BUTTON_HOLD(key) CGameInstance::GetInstance()->KeyButton_Pressing(key)
#define KEY_BUTTON_DOWN(key) CGameInstance::GetInstance()->KeyButton_Down(key)
#define KEY_BUTTON_UP(key) CGameInstance::GetInstance()->KeyButton_Up(key)
#define KEY_BUTTON_NONE(key) CGameInstance::GetInstance()->KeyButton_None(key)

#define MOUSE_LBUTTON_HOLD CGameInstance::GetInstance()->Mouse_Pressing(MOUSEKEYSTATE::LB)
#define MOUSE_RBUTTON_HOLD CGameInstance::GetInstance()->Mouse_Pressing(MOUSEKEYSTATE::RB)
#define MOUSE_WHEELBUTTON_HOLD CGameInstance::GetInstance()->Mouse_Pressing(MOUSEKEYSTATE::WHEEL)

#define MOUSE_LBUTTON_DOWN CGameInstance::GetInstance()->Mouse_Down(MOUSEKEYSTATE::LB)
#define MOUSE_RBUTTON_DOWN CGameInstance::GetInstance()->Mouse_Down(MOUSEKEYSTATE::RB)
#define MOUSE_WHEELBUTTON_DOWN CGameInstance::GetInstance()->Mouse_Down(MOUSEKEYSTATE::WHEEL)

#define MOUSE_LBUTTON_UP CGameInstance::GetInstance()->Mouse_Up(MOUSEKEYSTATE::LB)
#define MOUSE_RBUTTON_UP CGameInstance::GetInstance()->Mouse_Up(MOUSEKEYSTATE::RB)
#define MOUSE_WHEELBUTTON_UP CGameInstance::GetInstance()->Mouse_Up(MOUSEKEYSTATE::WHEEL)

#define GET_MOUSE_MOVE_X CGameInstance::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::X)
#define GET_MOUSE_MOVE_Y CGameInstance::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::Y)
#define GET_MOUSE_MOVE_WHEEL CGameInstance::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::WHEEL)
#pragma endregion

enum class CameraType;
struct DelegateHandle;
struct Camera_Cinematic_Sequence;
class CCollider;
class CGameObject;
class CObjectPool;
class CCameraMan;
class CLayer;
class CFxEffectAsset;
class CFxShaderVariant;
class CEffectHandler;


class ENGINE_DLL CGameInstance final : public CBase
{
	using Super = CBase;
private:
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

	void Reseed();
public:
#pragma region ENGINE
	HRESULT					Initialize_Engine(const ENGINE_DESC& Engine_Desc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void					Update_Engine(_float fTimeDelta);
	HRESULT					Draw_Begin(const Vec4* pClearColor);
	HRESULT					Draw();
	HRESULT					Draw_End();
	HRESULT					Resize_Viewport(D3D11_VIEWPORT viewport);
	HRESULT					Copy_BackBufferTexture(ID3D11Texture2D** ppTexture);
	void					Clear(_uint iLevelID);
	void					Destroy_Engine();
#pragma endregion

#pragma region PICKING
	void					PickingUpdate_ForTool(const Vec4& _vNDC);
	void					PickingUpdate(const _float fWinCX, const _float fWinCY);
	void					TransformRayToLocalSpace(const Matrix& matInvWorld);
	_bool					IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
	_bool					IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
	_bool					IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut);
	_bool					IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut);
	_bool					IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut);
	_bool					IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut);
	_bool					IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut);
	_bool					IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut);

	const Vec3&				Picking_Get_RayPos(bool isLocal = false) const;
	const Vec3&				Picking_Get_RayDir(bool isLocal = false) const;

#pragma endregion

#pragma region SHADERASSET_MANAGER
	CFxEffectAsset* GetOrCreate_FxEffectAsset(const path& filePath);
	CFxShaderVariant* GetOrCreate_Variant(const path& filePath, EVtxLayout eVertexLayoutID);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT					Immediately_ChangeLevel(_uint iNewLevelID, class CLevel* pNewLevel);
	void					Request_ChangeLevel(_uint iNewLevelID, class CLevel* pNewLevel);
	_bool					Is_Awaked(const _uint iLevelID) const;
	void					Awake_Level();
	class CLevel*			Get_CurrentLevel();
	_uint					Get_CurrentLevelIndex() const;
#pragma endregion

#pragma region TIMER_MANAGER
	_float					Get_TimeDelta(const _tchar* pTimerTag);
	void					Set_MaxTimeDelta(const _tchar* pTimerTag, _float fMaxTimeDelta);
	HRESULT					Add_Timer(const _tchar* pTimerTag);
	void					Remove_Timer(const _tchar* pTimerTag);
	void					Compute_TimeDelta(const _tchar* pTimerTag);
	void					Clear_Timers();
	class CTimer*			Find_Timer(const _tchar* pTimerTag);
#pragma endregion

#pragma region TIMESCALE_MANAGER
	void					Request_HitStop(_float fUnscaledDurationTime = HITSTOP_TIME);
	void					Request_SloMo(_float fScale, _float fUnscaledDurationTime);
	void					Active_SloMo(_float fScale);
	void					Deactivate_SloMo();
	void					Set_GlobalScale(_float fScale);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	CBase*					Find_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag);
	HRESULT					Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, CBase* pPrototype);
	CBase*					Clone_Prototype(EPrototypeType ePrototypeID, _uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT					Awake_GameObjects(const _uint iCurrentLevelID);
	
	// 파라미터 오브젝트를 레이어에 넣음
	CGameObject*			Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo);
	// 오브젝트를 생성해서 레이어에 넣음
	CGameObject*			Add_GameObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag,
							_uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg = nullptr);
	// 호출 시점에 바로 삭제
	void					Immediately_DeleteGameObject(_uint iCloneLevelIndex, CGameObject* pGo);
	// 호출 시점에 바로 회수
	void					Immediately_DespawnGameObject(_uint iCloneLevelIndex, CGameObject* pGo);

	// 파라미터 오브젝트를 EventManager에 Desc 전달하여 다음 프레임에 Layer에 추가
	void					Request_AddObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo, std::function<void(CGameObject*)> onSpawnedCallback = nullptr);
	// 오브젝트를 생성해서 EventManager에 Desc 전달하여 다음 프레임에 Layer에 추가
	void					Request_AddObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag,
							_uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg = nullptr, std::function<void(CGameObject*)> onSpawnedCallback = nullptr);
	// 풀 오브젝트를 스폰하여 EventManager에 Desc 전달하여 다음 프레임에 Layer에 추가
	void					Request_AddObject(_uint iPoolLevelIndex, const wstring& wstrPoolTag, _uint iSpawnLevelIndex, void* pArg, std::function<void(CGameObject*)> onSpawnedCallback = nullptr);
	// 해당 오브젝트를 EventManager에 Desc전달 하여 당므 프레임에  레이어에서 삭제 / 풀회수
	void					Request_DeleteGameObject(_uint iCloneLevelIndex, CGameObject* pGo);

	CGameObject*			Get_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, _uint iObjectIndex);
	CGameObject*			Get_GameObject_Front(_uint iLevelIndex, const wstring& wstrLayerTag);
	CGameObject*			Get_GameObject_Back(_uint iLevelIndex, const wstring& wstrLayerTag);
	list<CGameObject*>*		Get_GameObject_List(_uint iLevelIndex, const wstring& wstrLayerTag);
	
	void					Clear_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);
#pragma endregion

#pragma region OBJECTPOOL_MANAGER
	HRESULT Regist_Pool(_uint iTargetLevelIndex, const wstring& wstrPoolTag, const wstring& wstrLayerTag, _uint iSeedLevelID, const wstring &wstrSeedPrototypeTag,void* pArg, _uint iPoolCapacityCount);
	void Collect_PoolTags(_uint iLevelIndex, OUT vector<wstring>& vecOutTags) const;
	_int Get_ObjectPoolCapacity(_uint iLevelIndex, const wstring& wstrPoolTag) const;
	_int Get_ObjectPoolActiveCount(_uint iLevelIndex, const wstring& wstrPoolTag) const;
#ifdef _DEBUG
	
#endif
#pragma endregion

#pragma region COLLISION_MANAGER
	void					Collision_Check_Group(_uint iLeft, _uint iRight);
	HRESULT					Register_Collider(CCollider* pCollider);
	HRESULT					Unregister_Collider(CCollider* pCollider);
#pragma endregion

#pragma region CAMERA_MANAGER
	CCameraMan* Get_MainCamera();
	void Change_MainCamera(CameraType eType, const wstring& wstrTag);
	void Add_Camera(CameraType eType, const wstring& wstrTag, class CCameraMan* pGo);
	void Remove_Camera(CameraType eType, const wstring& wstrTag);
	void Add_Actor_Object(CGameObject* pGo, _bool bImmediatelyChange = false);
	void Remove_Actor_Object(CGameObject* pGo);
	void Change_Target(CGameObject* pGo);
	HRESULT Change_Target_Next();
	const Matrix& Get_ViewMatrix() const;
	void Set_ViewMatrix(Matrix matView);
	const Matrix& Get_ProjMatrix() const;
	void Set_ProjMatrix(Matrix matProj);
	const Matrix& Get_UI_ViewMatrix() const;
	const Matrix& Get_UI_ProjMatrix() const;
	ID3D11Buffer* Get_Global_ConstantBuffer();
	ID3D11Buffer* Get_Inv_ConstantBuffer();
	void Setup_ViewProj_ToCBuffer();
	void Setup_UIViewProj_ToCBuffer();
	void Setup_Inv_ToCBuffer();

	HRESULT Play_CameraCinematic(const Camera_Cinematic_Sequence* pCameraCinematicSequence);
	HRESULT Camera_Shaking(const CAM_SHAKING_DATA& tData);
#pragma endregion
	
#pragma region SOUND_MANAGER
	HRESULT Load_Sounds(const std::wstring& wstrFolderPath);
	void PlayBGM(const _tchar* pSoundKey, _float fVolume, _bool bLoop = true, _float fPitch = 1.f);
	void PlayAmbient(const _tchar* pSoundKey, _float fVolume, _bool bLoop = true, _float fPitch = 1.f);
	void Play_OneShot(const _tchar* pSoundKey, _float fVolume, _float fPitch = 1.f);
	void Play_RandOneShot(const _tchar* pSoundKey, _float fVolume, _int iCount, _float fPitch = 1.f);
	void Play_Controlled(const _tchar* pSoundKey, _uint iControlledId, _float fVolume, _bool  bLoop = false, _float fPitch = 1.f);
	void Stop_Controlled(_uint iControlledId);
	void Set_ControlledVolume(_uint iControlledId, _float fVolume);
	void Set_ControlledPitch(_uint iControlledId, _float fPitch);
	void Stop_All();
#pragma endregion

#pragma region INPUT_MANAGER
	_bool KeyButton_Pressing(_ubyte key);
	_bool KeyButton_Down(_ubyte key);
	_bool KeyButton_Up(_ubyte key);
	_bool KeyButton_None(_ubyte eKeyID);

	_bool Mouse_Down(MOUSEKEYSTATE eMouseKeyID);
	_bool Mouse_Up(MOUSEKEYSTATE eMouseKeyID);
	_bool Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	const POINT& Get_MousePos();
	_bool ShouldIgnoreMouseDelta() noexcept;
	void Request_CursorMode(ECursorMode eMode) noexcept;
	void Force_ReleaseCursor() noexcept;
#pragma endregion

#pragma region RESOURCE_MANAGER
	template<typename T>
	T* Load_Resource(const wstring& wstrKey, void* pArg);
	template<typename T>
	HRESULT Add_Resource(const wstring& wstrKey, T* pResource);
	template<typename T>
	T* Get_Resource(const wstring& wstrKey);
	template<typename T>
	void Remove_Resource(const wstring& wstrKey);
	class CTextureBase* GetOrAddTexture(const wstring& wstrKey, void* pArg);
#pragma endregion

#pragma region DATA_REPOSITORY
	HRESULT Load_Folder_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath);
	HRESULT Load_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath);
	HRESULT Save_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath) const;
	CDataDocumentBase* Ensure_Document(_uint iLevelID, DTO::ECategory eCategory, const path& filePath);
	const CDataDocumentBase* Get_Document(_uint iLevelID, DTO::ECategory eCategory, const string& strFileKey);

	template<typename T>
	HRESULT Regist_Document(_uint iLevelID, DTO::ECategory eCategory);
#pragma endregion

#pragma region OCTREE_MANAGER
	HRESULT Register_Octree(CGameObject* pGo, RENDER_CATEGORY eCategory, const BoundingBox& AABB, _bool bDynamic = false);
	void Unregister(CGameObject* pGo);
	HRESULT Ready_Octree(const OCTREE_DESC& desc);
#pragma endregion

#pragma region RENDER_MANAGER
	inline void Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO);
#ifdef _DEBUG
	inline void Push_DebugComponent(class CComponent* pComp);
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
	ID3D11Buffer* Get_Light_ConstantBuffer();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Push_DynamicLight(class CLight* pLight);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect_Tex* pVIBuffer);
	class CLight* Get_Light(LIGHT_TYPE eType, _uint iIndex = 0);
	void Clear_Lights();
#pragma endregion

#pragma region EVENT_MANAGER
	void Flush_All();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Request_DrawFont(FONT_DESC Desc);
	HRESULT Render_Fonts();
#pragma endregion

#pragma region EVENTBUS_MANAGER
	template<typename Tag, typename Func>
	DelegateHandle Subscribe(Func&& func);
	template<typename Tag>
	DelegateHandle Subscribe(typename Tag::Signature* pFunc);
	template<typename Tag, typename T, typename ...Args>
	DelegateHandle Subscribe(T* pObj, void (T::* memFunc)(Args ...));
	template<typename Tag, typename T, typename ...Args>
	DelegateHandle Subscribe(T* pObj, void(T::* memFunc)(Args ...) const);
	template<typename Tag>
	bool Unsubscribe(DelegateHandle handle);
	template<typename Tag, typename... Arg>
	void Broadcast(Arg&&... a);
	template<class Tag>
	void Clear_Channel();
#pragma endregion

#pragma region FRUSTRUM
	void Ready_Frustrum(); 
	_float Get_FrustrumMidStart() const;
	_float Get_FrustrumFarStart() const;
	void Resize_SplitFrustrum(const _float fMidStart, const _float fFarStart);
	EFrustrumTier Classify_BySplitFrustrum(const BoundingBox &AABB);
	EFrustrumTier Classify_BySplitFrustrum(const BoundingSphere& Sphere);
	BoundingFrustum* Get_BoundingFrustrum_Local();
	BoundingFrustum* Get_BoundingFrustrum_World();
#pragma endregion

#pragma region RENDERTARGET_MANAGER
	HRESULT Add_RenderTarget(ERenderTarget eTarget, const CRenderTarget::RENDERTARGET_DESC* pDesc);
	HRESULT Add_MRT(EMRTLayer eMRTLayer, ERenderTarget eTarget);
	HRESULT Begin_MRT(EMRTLayer eMRTLayer, _bool bClear = true, _bool bUseDSV = true);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(ERenderTarget eTarget, class CShader* pShader);
	HRESULT Copy_SceneHDRResource(ERenderTarget eTarget);
#ifdef _DEBUG
	HRESULT Ready_RT_Debug(ERenderTarget eTarget, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_RT_Render(EMRTLayer eMRTLayer, class CShader* pShader, class CVIBuffer_Rect_Tex* pVIBuffer);
	ID3D11ShaderResourceView* Get_RenderTargetSRV(ERenderTarget eTarget);
	SHADER_SSAOPARAM_DESC& Get_SSAOParamDesc();
	const SHADER_SSAOPARAM_DESC& Get_SSAOParamDesc() const;
	HRESULT Commit_SSAOParam();
	SHADER_HDRPARAM_DESC& Get_HDRParamDesc();
	const SHADER_HDRPARAM_DESC& Get_HDRParamDesc() const;
	HRESULT Commit_HDRParam();
	SHADER_BLOOMPARAM_DESC& Get_BloomParamDesc();
	const SHADER_BLOOMPARAM_DESC& Get_BloomParamDesc() const;
	HRESULT Commit_BloomParam();
	SHADER_OUTLINE_DESC& Get_OutlineParamDesc();
	const SHADER_OUTLINE_DESC& Get_OutlineParamDesc() const;
	HRESULT Commit_OutlineParam();
	HRESULT Commit_AllPostParams();
#endif
#pragma endregion

#pragma region RANDOM
	float Rand_Float(float fA, float fB)
	{
		return std::uniform_real_distribution<float>(fA, fB)(m_rng);
	}

	int Rand_Int(int iA, int iB)
	{
		return std::uniform_int_distribution<int>(iA, iB)(m_rng);
	}
#pragma endregion

#pragma region PHYSICS_MODULE
	void StepPhysics(_float fTimeDelta);
	void AddActor(PxRigidActor* actor);
	void AddRagdoll(PxArticulationReducedCoordinate* pArticulation);
	void ClearPhysics();
	void FlushScene();
	void RemoveActor(PxRigidActor* actor);
	void ResetActorFilter(PxRigidActor* actor);
	PxScene* GetPhysicsScene();
	PxControllerManager* GetPhysicsCCTManager();
	PxTransform XMMatrixToPxTransform(Matrix mat);
	Matrix PxTransformToXMMatrix(PxTransform pxTransform);
	_bool Execute_Overlap(PxGeometry& shape, PxTransform& transform, OUT PxOverlapBuffer& hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback);
	class CPhysics_QueryFilterCallback* GetQueryFilterCallback();
	class CPhysics_QueryFilterCallback_Gun* GetQueryFilterCallback_Gun();
	void SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes);
	PxCollection* DeserializeStaticMesh(std::filesystem::path path);
	void SerializeConvexMesh(std::filesystem::path path, vector<PxConvexMesh*> meshes);
	PxCollection* SerializeConvexMesh(std::filesystem::path path);
	void SerializeLevel(std::filesystem::path path) {}
	void DeserializeLevel(std::filesystem::path path) {}
	PxMaterial* GetPhysicsMaterial(EPhysicsMaterial eMaterial);
	vector<PxShape*> GetShape(PHYSICSCOLLIDER_DESC* pDesc);
	vector<PxShape*> GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc);
	vector<PxShape*> CopyShapes(vector<PxShape*>& shapes);
	vector<PxRigidActor*> GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
	RAGDOLLELEMENTS CreateRagdoll(array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc);
	PxController* GetController(PHYSICSCCT_DESC* pDesc);
	class CPhysics_CCTFilterCallback* GetCCTFilterCallback();
	void RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag);
	PxQuat GetPureRotation(const Matrix& mat);
	PxVec3 GetPureScale(const Matrix& mat);
	void Overlap_EventCallback(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc);
	void Raycast_EventCallback(CGameObject* pOwner, PxRaycastBuffer* pRaycastHitBuffer, CPhysicsAttackRaycast::ATTACKRAYCASTDESC* raycastDesc);
	_bool RayCast(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall);
#ifdef _DEBUG
	void Physics_Render(PxRigidActor* pActor, XMVECTOR color = DirectX::Colors::White);
	void Physics_Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color = DirectX::Colors::White);
#endif // _DEBUG
#pragma endregion

#pragma region EFFECT_MANAGER
	void Request_Effect(CEffectHandler* handler, const std::string& UniqueEffectName, const std::string& strTag, EFFECT_SPAWN_DESC& Desc);
	void Request_Effect(const std::string& strTag, EFFECT_SPAWN_DESC& Desc);

	void Push_EffectData(_uint iHashTag, void* Desc);
	void* Find_EffectData(_uint iHashTag);
#pragma endregion

#pragma region JUDGEMENT_SYSTEM
	void Push_CollidedData(const COLLIDED_DESC& desc);
#pragma endregion

// Todo - 쓰레기통 정리
#pragma region GAMEDATA_MANAGER
	HRESULT		Register_GlobalEventsBroadCast(_uint iTypeIndex, std::function<void()> funcGlobalEvent);
	HRESULT		BroadCaset_RegisterGlobalEvent(_uint iTypeIndex);
#pragma region TextureSplating
	HRESULT		GameDataManager_Load_TextureSplatingInfoData();
	/* 이름으로 Binding 하는 함수 */
	HRESULT		GameDataManager_Bind_SplatingTextureInfo(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName);
#pragma endregion

#pragma region
	HRESULT		GameDataManager_Load_CameraCinematicSequence();
	HRESULT		GameDataManager_Save_CameraCinematicSequence();
	HRESULT		GameDataManager_Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT struct Camera_Cinematic_Sequence* pOutCamCinematicSequence);
	HRESULT		GameDataManager_Save_CameraCinematicSequence(const wstring& wstrFindKey, const struct Camera_Cinematic_Sequence* pSaveCamCinematicSequence);

	HRESULT		Play_CameraCinematic(const wstring& wstrFindKey);
	vector<std::string> GameDataManager_Get_CameraCinematicSequenceNames() const;


#pragma endregion

	const DTO::TAttackPreset_Data* Find_AttackPrseet(_uint iPresetKey) const;
	const DTO::TAttackPreset_Data* Find_AttackPresetByTag(const string& strTag) const;
	_uint Get_AttackPresetIdByTag(const string& strTag) const;
	HRESULT Upsert_AttackPresetData(const DTO::TAttackPreset_Data& inData);
	const unordered_map<_uint, DTO::TAttackPreset_Data>& Get_AttackPresetsData_ForDebug() const;
#pragma endregion

public:
	void SetChangeLevelSequence(_bool bVal) { m_bChangeLevelSequence = bVal; }
	_bool Is_ChangeLevelSequence() { return m_bChangeLevelSequence; }
	void SetDestroyEngineSequence(_bool bVal) { m_bDestroyEngineSequence = bVal; }
	_bool Is_DestroyEngineSequence() { return m_bDestroyEngineSequence; }
	_bool Is_TearDownSequence() { return m_bChangeLevelSequence || m_bDestroyEngineSequence; }
private:
	class CObjectPool_Manager* m_pObjectPool_Manager = { nullptr };
	class CDataRepository* m_pDataRepository = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CTimeScale_Manager* m_pTimeScale_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	class COctree_Manager* m_pOctree_Manager = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CCollision_Manager* m_pCollision_Manager = { nullptr };
	class CRender_Manager* m_pRender_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
	class CCamera_Manager* m_pCamera_Manager = { nullptr };
	class CInput_Manager* m_pInput_Manager = { nullptr };
	class CResource_Manager* m_pResource_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CEvent_Manager* m_pEvent_Manager = { nullptr };
	class CEventBus_Manager* m_pEventBus_Manager = { nullptr };
	class CGameDataManager* m_pGameData_Manager = { nullptr };
	class CRenderTarget_Manager* m_pRenderTarget_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };
	class CFrustrum* m_pFrustrum = { nullptr };
	class CShaderAsset_Manager* m_pShaderAsset_Manager = { nullptr };
	class CPhysics_Module* m_pPhysics_Module = { nullptr };
	class CEffect_Manager* m_pEffect_Manager = { nullptr };
	class CJudgementSystem* m_pJudgementSystem = { nullptr };
private:
	std::mt19937_64 m_rng;

	_bool m_bChangeLevelSequence = { false };
	_bool m_bDestroyEngineSequence = { false };
public:
	virtual void			Free() override;

	friend class CRender_Manager;
};

#pragma region RESOURCE_MANAGER
template<typename T>
inline T* CGameInstance::Load_Resource(const wstring& wstrKey, void* pArg)
{
	return m_pResource_Manager->Load<T>(wstrKey, pArg);
}

template<typename T>
inline HRESULT CGameInstance::Add_Resource(const wstring& wstrKey, T* pResource)
{
	return m_pResource_Manager->Add<T>(wstrKey, pResource);
}

template<typename T>
inline T* CGameInstance::Get_Resource(const wstring& wstrKey)
{
	return m_pResource_Manager->Get<T>(wstrKey);
}
template<typename T>
inline void CGameInstance::Remove_Resource(const wstring& wstrKey)
{
	return m_pResource_Manager->Remove<T>(wstrKey);
}
#pragma endregion

#pragma region EVENTBUS_MANAGER
template<typename Tag, typename Func>
inline DelegateHandle CGameInstance::Subscribe(Func&& func)
{
	return m_pEventBus_Manager->Add_Lambda<Tag>(std::forward<Func>(func));
}

template<typename Tag>
inline DelegateHandle CGameInstance::Subscribe(typename Tag::Signature* pFunc)
{
	return m_pEventBus_Manager->Add_Static<Tag>(pFunc);
}

template<typename Tag, typename T, typename ...Args>
inline DelegateHandle CGameInstance::Subscribe(T* pObj, void (T::* memFunc)(Args ...))
{
	return m_pEventBus_Manager->Add_MemFunc<Tag>(pObj, memFunc);
}

template<typename Tag, typename T, typename ...Args>
inline DelegateHandle CGameInstance::Subscribe(T* pObj, void (T::* memFunc)(Args ...) const)
{
	return m_pEventBus_Manager->Add_MemFunc<Tag>(pObj, memFunc);
}

template<typename Tag>
inline bool CGameInstance::Unsubscribe(DelegateHandle handle)
{
	return m_pEventBus_Manager->Unsubscribe<Tag>(handle);
}

template<typename Tag, typename... Arg>
inline void CGameInstance::Broadcast(Arg&&... a)
{
	m_pEventBus_Manager->Broadcast<Tag>(std::forward<Arg>(a)...);
}

template<typename Tag>
inline void CGameInstance::Clear_Channel()
{
	return m_pEventBus_Manager->Clear_Channel<Tag>();
}
#pragma endregion

#pragma region DATA_REPOSITORY
template<typename T>
inline HRESULT CGameInstance::Regist_Document(_uint iLevelID, DTO::ECategory eCategory)
{
	return m_pDataRepository->Regist_Category<T>(iLevelID, eCategory);
}

#pragma endregion 

NS_END