#include "Engine_pch.h"
#include "GameInstance.h"
#include "Frustrum.h"
#include "Font_Manager.h"
#include "Event_Manager.h"
#include "ObjectPool_Manager.h"
#include "Octree_Manager.h"
#include "GameDataManager.h"
#include "Collision_Manager.h"
#include "Constant_Buffer.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Level.h"
#include "Timer.h"
#include "Picking.h"
#include "RenderTarget_Manager.h"
#include "Timer_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "TimeScale_Manager.h"
#include "CameraMan.h"
#include "Camera_Manager.h"
#include "Level_Manager.h"
#include "ShaderAsset_Manager.h"
#include "DataRepository.h"
#include "Input_Manager.h"
#include "Graphic_Device.h"
#include "Render_Manager.h"
#include "Physics_Module.h"
#include "Effect_Manager.h"
#include "EffectHandler.h"
#include "GameData_Struct.h"
#include "JudgementSystem.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

void CGameInstance::Reseed()
{
	std::random_device randomDevice;
	uint64_t t = (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
	uint64_t seed = (t << 1) ^ ((uint64_t)randomDevice() << 32) ^ randomDevice();
	m_rng.seed(seed);
}

#pragma region ENGINE
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& Engine_Desc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	if (!(m_pTimer_Manager = CTimer_Manager::Create()))
		return E_FAIL;

	if (!(m_pTimeScale_Manager = CTimeScale_Manager::Create()))
		return E_FAIL;

	if (!(m_pGraphic_Device = CGraphic_Device::Create(Engine_Desc, ppDevice, ppContext)))
		return E_FAIL;

	if (!(m_pCollision_Manager = CCollision_Manager::Create(Engine_Desc.iCollideLayerCount)))
		return E_FAIL;

	if(!(m_pGameData_Manager = CGameDataManager::Create(*ppDevice , *ppContext)))
		return E_FAIL;

	if (!(m_pDataRepository = CDataRepository::Create(Engine_Desc.iLevelCount)))
		return E_FAIL;

	if (!(m_pLevel_Manager = CLevel_Manager::Create()))
		return E_FAIL;

	if (!(m_pPrototype_Manager = CPrototype_Manager::Create(Engine_Desc.iLevelCount)))
		return E_FAIL;

	if (!(m_pObjectPool_Manager = CObjectPool_Manager::Create(Engine_Desc.iLevelCount)))
		return E_FAIL;

	if (!(m_pObject_Manager = CObject_Manager::Create(Engine_Desc.iLevelCount, m_pObjectPool_Manager)))
		return E_FAIL;

	if (!(m_pInput_Manager = CInput_Manager::Create(Engine_Desc.hInst, Engine_Desc.hWnd)))
		return E_FAIL;

	if (!(m_pResource_Manager = CResource_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pCamera_Manager = CCamera_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pRender_Manager = CRender_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pSound_Manager = CSound_Manager::Create()))
		return E_FAIL;

	if (!(m_pShaderAsset_Manager = CShaderAsset_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pRender_Manager->Set_ShaderResources()))
		return E_FAIL;

	if (!(m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pFrustrum = CFrustrum::Create()))
		return E_FAIL;

	if (!(m_pEvent_Manager = CEvent_Manager::Create()))
		return E_FAIL;

	if (!(m_pPicking = CPicking::Create(Engine_Desc.hWnd)))
		return E_FAIL;

	if (!(m_pEventBus_Manager = CEventBus_Manager::Create()))
		return E_FAIL;

	if (!(m_pPhysics_Module = CPhysics_Module::Create(*ppDevice, *ppContext)))
		return E_FAIL;

	if (!(m_pOctree_Manager = COctree_Manager::Create()))
		return E_FAIL;

	if (!(m_pEffect_Manager = CEffect_Manager::Create()))
		return E_FAIL;
	
	if (!(m_pJudgementSystem = CJudgementSystem::Create()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	_float fUnscaledTimeDelta = fTimeDelta;
	_float fScaledTimeDelta = m_pTimeScale_Manager->Begin_Frame(fUnscaledTimeDelta);

	m_pSound_Manager->Update();
	m_pInput_Manager->Update();
	m_pLevel_Manager->Update(fUnscaledTimeDelta);
	m_pObject_Manager->Update_Priority(fUnscaledTimeDelta, fScaledTimeDelta);
	m_pObject_Manager->Update(fUnscaledTimeDelta, fScaledTimeDelta);
	m_pObject_Manager->Update_Late(fUnscaledTimeDelta, fScaledTimeDelta);

	// 피직스 시뮬레이트
	if(fScaledTimeDelta > g_XMEpsilon.f[0])
		m_pPhysics_Module->StepPhysics(fScaledTimeDelta);

	m_pJudgementSystem->Flush_CollidedEvent();

	// 메인카메라 업데이트#ifdef _DEBUG
	m_pCamera_Manager->Update_ViewMatrix();
	m_pFrustrum->Update();

	m_pLevel_Manager->Update_Picking();

	// 업데이트 후 마지막
	m_pObject_Manager->Ready_Before_Render(fUnscaledTimeDelta, fScaledTimeDelta);
}

HRESULT CGameInstance::Draw_Begin(const Vec4* pClearColor)
{
	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(&m_pLevel_Manager->Get_ClearColor())))
		return E_FAIL;
	
	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRender_Manager->Render()))
	{
		MSG_BOX("CGameInstance::Draw, Failed");
		return E_FAIL;
	}

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameInstance::Draw_End()
{
	return m_pGraphic_Device->Present();
}
HRESULT CGameInstance::Resize_Viewport(D3D11_VIEWPORT viewport)
{
	return m_pGraphic_Device->Resize(viewport);
}
HRESULT CGameInstance::Copy_BackBufferTexture(ID3D11Texture2D** ppTexture)
{
	return m_pGraphic_Device->Copy_BackBufferTexture(ppTexture);
}
void CGameInstance::Clear(_uint iLevelID)
{
	m_pRender_Manager->Clear();
	m_pDataRepository->Clear(iLevelID);
	m_pObjectPool_Manager->All_Despawn_StaticLevel();
	m_pTimeScale_Manager->Clear();
	m_pOctree_Manager->Clear();
	m_pObject_Manager->Clear(iLevelID);
	m_pObjectPool_Manager->Clear(iLevelID);
	m_pPrototype_Manager->Clear(iLevelID);
	m_pInput_Manager->Clear();
	m_pCamera_Manager->Clear();
	m_pEventBus_Manager->Clear_All();
	m_pSound_Manager->StopAll();
}

#pragma region PICKING
void CGameInstance::PickingUpdate_ForTool(const Vec4& _vNDC)
{
	m_pPicking->Update(_vNDC);
}

void CGameInstance::PickingUpdate(const _float fWinCX, const _float fWinCY)
{
	m_pPicking->Update(fWinCX, fWinCY);
}

void CGameInstance::TransformRayToLocalSpace(const Matrix& matInvWorld)
{
	m_pPicking->TransformRayToLocalSpace(matInvWorld);
}

_bool CGameInstance::IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithTriangle_World(vPointA, vPointB, vPointC, vOut);
}

_bool CGameInstance::IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithTriangle_Local(vPointA, vPointB, vPointC, vOut);
}
_bool CGameInstance::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithAABB_World(pDesc, vOut);
}
_bool CGameInstance::IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithAABB_Local(pOriginDesc, vOut);
}
_bool CGameInstance::IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithOBB_World(pDesc, vOut);
}
_bool CGameInstance::IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithOBB_Local(pOriginDesc, vOut);
}
_bool CGameInstance::IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithSphere_World(pDesc, vOut);
}
_bool CGameInstance::IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut)
{
	return m_pPicking->IntersectrayWithSphere_Local(pOriginDesc, vOut);
}

const Vec3& CGameInstance::Picking_Get_RayPos(bool isLocal) const
{
	return m_pPicking->Get_RayPos(isLocal);
}

const Vec3& CGameInstance::Picking_Get_RayDir(bool isLocal) const
{
	return m_pPicking->Get_RayDir(isLocal);
}
#pragma endregion

#pragma region SHADERASSET_MANAGER
CFxEffectAsset* CGameInstance::GetOrCreate_FxEffectAsset(const path& filePath)
{
	return m_pShaderAsset_Manager->GetOrCreate_FxEffectAsset(filePath);
}
CFxShaderVariant* CGameInstance::GetOrCreate_Variant(const path& filePath, EVtxLayout eVertexLayoutID)
{
	return m_pShaderAsset_Manager->GetOrCreate_Variant(filePath, eVertexLayoutID);
}
#pragma endregion 

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Immediately_ChangeLevel(_uint iNewLevelID, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iNewLevelID, pNewLevel);
}

void CGameInstance::Request_ChangeLevel(_uint iNewLevelID, CLevel* pNewLevel)
{
	if (!pNewLevel)
		return;

	ChangeLevelEventDesc desc = {};
	desc.iNewLevelID = static_cast<_int>(iNewLevelID);
	desc.pNewLevel = pNewLevel;
	m_pEvent_Manager->Push_ChangeLevelEvet(desc);
}

_bool CGameInstance::Is_Awaked(const _uint iLevelID) const
{
	return m_pLevel_Manager->Is_Awaked(iLevelID);
}

void CGameInstance::Awake_Level()
{
	m_pLevel_Manager->Awake();
}

CLevel* CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

_uint CGameInstance::Get_CurrentLevelIndex() const
{
	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}
void CGameInstance::Set_MaxTimeDelta(const _tchar* pTimerTag, _float fMaxTimeDelta)
{
	m_pTimer_Manager->Set_MaxTimeDelta(pTimerTag, fMaxTimeDelta);
}
HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Remove_Timer(const _tchar* pTimerTag)
{
	m_pTimer_Manager->Remove_Timer(pTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _tchar* pTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

void CGameInstance::Clear_Timers()
{
	m_pTimer_Manager->Clear_Timers();
}

CTimer* CGameInstance::Find_Timer(const _tchar* pTimerTag)
{
	return m_pTimer_Manager->Find_Timer(pTimerTag);
}
#pragma endregion

#pragma region TIMESCALE_MANAGER
void CGameInstance::Request_HitStop(_float fUnscaledDurationTime)
{
	m_pTimeScale_Manager->Request_HitStop(fUnscaledDurationTime);
}
void CGameInstance::Request_SloMo(_float fScale, _float fUnscaledDurationTime)
{
	m_pTimeScale_Manager->Request_SloMo(fScale, fUnscaledDurationTime);
}
void CGameInstance::Active_SloMo(_float fScale)
{
	m_pTimeScale_Manager->Active_SloMo(fScale);
}
void CGameInstance::Deactivate_SloMo()
{
	m_pTimeScale_Manager->Deactivate_SloMo();
}
void CGameInstance::Set_GlobalScale(_float fScale)
{
	m_pTimeScale_Manager->Set_GlobalScale(fScale);
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
CBase* CGameInstance::Find_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag)
{
	return m_pPrototype_Manager->Find_Prototype(iLevelIndex, wstrPrototypeTag);
}
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, wstrPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(EPrototypeType ePrototypeID, _uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototypeID, iLevelIndex, wstrPrototypeTag, pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Awake_GameObjects(const _uint iCurrentLevelID)
{
	return m_pObject_Manager->Awake(iCurrentLevelID);
}

CGameObject* CGameInstance::Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	return m_pObject_Manager->Add_GameObject(iCloneLevelIndex, wstrLayerTag, pGo);
}
CGameObject* CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, wstrPrototypeTag, iCloneLevelIndex, wstrLayerTag, pArg);
}
void CGameInstance::Immediately_DeleteGameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	m_pObject_Manager->Delete_GameObject(iCloneLevelIndex, wstrLayerTag, pGo);
}
void CGameInstance::Request_AddObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo, std::function<void(CGameObject*)> onSpawnedCallback)
{
	if (!pGo)
		return;

	SpawnEventDesc desc = {};
	desc.iCloneLevelIndex = static_cast<_int>(iCloneLevelIndex);
	desc.wstrLayerTag = wstrLayerTag;
	desc.pClone = pGo;
	if (onSpawnedCallback)
		desc.callback = std::move(onSpawnedCallback);
	m_pEvent_Manager->Push_SpawnEvent(desc);
}
void CGameInstance::Request_AddObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg, std::function<void(CGameObject*)> onSpawnedCallback)
{
	CGameObject* pResult = { nullptr };
	pResult = static_cast<CGameObject*>(m_pPrototype_Manager->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg));

	if (pResult)
		Request_AddObject(iCloneLevelIndex, wstrLayerTag, pResult, onSpawnedCallback);
}
void CGameInstance::Request_AddObject(_uint iPoolLevelIndex, const wstring& wstrPoolTag, _uint iSpawnLevelIndex, void* pArg, std::function<void(CGameObject*)> onSpawnedCallback)
{
	CGameObject* pResult = { nullptr };
	wstring wstrLayerTag = { L"" };
	pResult = m_pObjectPool_Manager->Spawn(iPoolLevelIndex, wstrPoolTag, wstrLayerTag, pArg);

	if(pResult)
		Request_AddObject(iSpawnLevelIndex, wstrLayerTag, pResult, onSpawnedCallback);
}
void CGameInstance::Request_DeleteGameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (!pGo)
		return;

	DespawnEventDesc desc = {};
	desc.iClonedLevelIndex = iCloneLevelIndex;
	desc.wstrLayerTag = wstrLayerTag;
	desc.pGo = pGo;
	m_pEvent_Manager->Push_DespawnEvent(desc);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, _uint iObjectIndex)
{
	return m_pObject_Manager->Get_GameObject(iLevelIndex, wstrLayerTag, iObjectIndex);
}
CGameObject* CGameInstance::Get_GameObject_Front(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	return m_pObject_Manager->Get_GameObject_Front(iLayerIndex, wstrLayerTag);
}
CGameObject* CGameInstance::Get_GameObject_Back(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	return m_pObject_Manager->Get_GameObject_Back(iLayerIndex, wstrLayerTag);
}
list<CGameObject*>* CGameInstance::Get_GameObject_List(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	return m_pObject_Manager->Get_GameObject_List(iLayerIndex, wstrLayerTag);
}
void CGameInstance::Clear_Layer(_uint iLevelIndex, const wstring& wstrLayerTag)
{
	m_pObject_Manager->Clear_Layer(iLevelIndex, wstrLayerTag);
}
#pragma endregion

#pragma region OBJECTPOOL_MANAGER
/// <summary>
/// 오브젝트 Pool에 등록하기 위한 함수 
/// </summary>
/// <param name="iTargetLevelIndex">	/ 생성할 Level Index </param>
/// <param name="wstrPoolTag">			/ 등록할 Pool Tag </param>
/// <param name="wstrLayerTag">			/ Clone해서 생성할 때 넣을 LayerTag Layer Tag </param>
/// <param name="iSeedLevelID">			/ 프로토타입으로 등록된 Level ID </param>
/// <param name="wstrSeedPrototypeTag"> / 프로토타입 태그 </param>
/// <param name="pArg">					/ Desc </param>
/// <param name="iPoolCapacityCount">	/ Pool에 넣을 </param>
/// <returns></returns>
HRESULT CGameInstance::Regist_Pool(_uint iTargetLevelIndex, const wstring& wstrPoolTag, const wstring& wstrLayerTag, _uint iSeedLevelID, const wstring& wstrSeedPrototypeTag, void* pArg, _uint iPoolCapacityCount)
{
	return m_pObjectPool_Manager->Regist_Pool(iTargetLevelIndex, wstrPoolTag, wstrLayerTag, iSeedLevelID, wstrSeedPrototypeTag, pArg, iPoolCapacityCount);
}
#pragma endregion

#pragma region COLLISION_MANAGER
/// <summary>
/// Level의 Awake() 시점에 호출 해야함
/// 두개의 Layer충돌을 검사하도록 등록하는 함수
/// </summary>
/// <param name="iLeft">LayerID_1</param>
/// <param name="iRight">LayerID_2</param>
void CGameInstance::Collision_Check_Group(_uint iLeft, _uint iRight)
{
	m_pCollision_Manager->Check_Group(iLeft, iRight);
}
HRESULT CGameInstance::Register_Collider(CCollider* pCollider)
{
	return m_pCollision_Manager->Register_Collider(pCollider);
}
HRESULT CGameInstance::Unregister_Collider(CCollider* pCollider)
{
	return m_pCollision_Manager->Unregister_Collider(pCollider);
}
#pragma endregion

#pragma region CAMERA_MANAGER

CCameraMan* CGameInstance::Get_MainCamera()
{
	return m_pCamera_Manager->Get_MainCamera();
}
void CGameInstance::Change_MainCamera(CameraType eType, const wstring& wstrTag)
{
	m_pCamera_Manager->Change_MainCamera(eType, wstrTag);
}
void CGameInstance::Add_Camera(CameraType eType, const wstring& wstrTag, CCameraMan* pGo)
{
	m_pCamera_Manager->Add_Camera(eType, wstrTag, pGo);
}
void CGameInstance::Remove_Camera(CameraType eType, const wstring& wstrTag)
{
	m_pCamera_Manager->Remove_Camera(eType, wstrTag);
}
void CGameInstance::Add_Actor_Object(CGameObject* pGo, _bool bImmediatelyChange)
{
	m_pCamera_Manager->Add_Actor_Object(pGo, bImmediatelyChange);
}
void CGameInstance::Remove_Actor_Object(CGameObject* pGo)
{
	m_pCamera_Manager->Remove_Actor_Object(pGo);
}
void CGameInstance::Change_Target(CGameObject* pGo)
{
	m_pCamera_Manager->Change_Target(pGo);
}
HRESULT CGameInstance::Change_Target_Next()
{
	return m_pCamera_Manager->Change_Target_Next();
}
const Matrix& CGameInstance::Get_ViewMatrix() const
{
	return m_pCamera_Manager->Get_ViewMatrix();
}
void CGameInstance::Set_ViewMatrix(Matrix matView)
{
	m_pCamera_Manager->Set_ViewMatrix(matView);
}
const Matrix& CGameInstance::Get_ProjMatrix() const
{
	return m_pCamera_Manager->Get_ProjMatrix();
}
void CGameInstance::Set_ProjMatrix(Matrix matProj)
{
	m_pCamera_Manager->Set_ProjMatrix(matProj);
}
const Matrix& CGameInstance::Get_UI_ViewMatrix() const
{
	return m_pCamera_Manager->Get_UI_ViewMatrix();
}
const Matrix& CGameInstance::Get_UI_ProjMatrix() const
{
	return m_pCamera_Manager->Get_UI_ProjMatrix();
}
ID3D11Buffer* CGameInstance::Get_Global_ConstantBuffer()
{
	return m_pCamera_Manager->Get_Global_ConstantBuffer();
}
ID3D11Buffer* CGameInstance::Get_Inv_ConstantBuffer()
{
	return m_pCamera_Manager->Get_Inv_ConstantBuffer();
}
void CGameInstance::Setup_ViewProj_ToCBuffer()
{
	m_pCamera_Manager->Setup_ViewProj_ToCBuffer();
}
void CGameInstance::Setup_UIViewProj_ToCBuffer()
{
	m_pCamera_Manager->Setup_UIViewProj_ToCBuffer();
}
void CGameInstance::Setup_Inv_ToCBuffer()
{
	m_pCamera_Manager->Setup_Inv_ToCBuffer();
}

HRESULT CGameInstance::Camera_Shaking(const CAM_SHAKING_DATA& tData)
{
	return m_pCamera_Manager->Camera_Shaking(tData);
}
#pragma endregion

#pragma region SOUND_MANAGER
HRESULT CGameInstance::Load_Sounds(const std::wstring& wstrFolderPath)
{
	return m_pSound_Manager->Load_Sounds(wstrFolderPath);
}
void CGameInstance::PlayBGM(const _tchar* pSoundKey, _float fVolume, _bool bLoop, _float fPitch)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume, bLoop, fPitch);
}
void CGameInstance::PlayAmbient(const _tchar* pSoundKey, _float fVolume, _bool bLoop, _float fPitch)
{
	m_pSound_Manager->Play_Controlled(pSoundKey, 1, fVolume, bLoop, fPitch);
}
void CGameInstance::Play_OneShot(const _tchar* pSoundKey, _float fVolume, _float fPitch)
{
	m_pSound_Manager->Play_OneShot(pSoundKey, fVolume, fPitch);
}
void CGameInstance::Play_RandOneShot(const _tchar* pSoundKey, _float fVolume, _int iCount, _float fPitch)
{
	m_pSound_Manager->Play_RandOneShot(pSoundKey, fVolume, iCount, fPitch);
}
void CGameInstance::Play_Controlled(const _tchar* pSoundKey, _uint iControlledId, _float fVolume, _bool bLoop, _float fPitch)
{
	m_pSound_Manager->Play_Controlled(pSoundKey, iControlledId, fVolume, bLoop, fPitch);
}
void CGameInstance::Stop_Controlled(_uint iControlledId)
{
	m_pSound_Manager->Stop_Controlled(iControlledId);
}
void CGameInstance::Set_ControlledVolume(_uint iControlledId, _float fVolume)
{
	m_pSound_Manager->Set_ControlledVolume(iControlledId, fVolume);
}
void CGameInstance::Set_ControlledPitch(_uint iControlledId, _float fPitch)
{
	m_pSound_Manager->Set_ControlledVolume(iControlledId, fPitch);
}
void CGameInstance::Stop_All()
{
	m_pSound_Manager->StopAll();
}
#pragma endregion

#pragma region INPUT_MANAGER
_bool CGameInstance::KeyButton_Pressing(_ubyte key)
{
	return m_pInput_Manager->KeyButton_Pressing(key);
}
_bool CGameInstance::KeyButton_Down(_ubyte key)
{
	return m_pInput_Manager->KeyButton_Down(key);
}
_bool CGameInstance::KeyButton_Up(_ubyte key)
{
	return m_pInput_Manager->KeyButton_Up(key);
}
_bool CGameInstance::KeyButton_None(_ubyte eKeyID)
{
	return m_pInput_Manager->KeyButton_None(eKeyID);
}
_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouseKeyID)
{
	return m_pInput_Manager->Mouse_Down(eMouseKeyID);
}
_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouseKeyID)
{
	return m_pInput_Manager->Mouse_Up(eMouseKeyID);
}
_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID)
{
	return m_pInput_Manager->Mouse_Pressing(eMouseKeyID);
}
_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Manager->Get_DIMouseMove(eMouseState);
}
const POINT& CGameInstance::Get_MousePos()
{
	return m_pInput_Manager->Get_MousePos();
}
void CGameInstance::Request_CursorMode(ECursorMode eMode) noexcept
{
	m_pInput_Manager->Request_CursorMode(eMode);
}
_bool CGameInstance::ShouldIgnoreMouseDelta() noexcept
{
	return m_pInput_Manager->ShouldIgnoreMouseDelta();
}
void CGameInstance::Force_ReleaseCursor() noexcept
{
	m_pInput_Manager->Force_ReleaseCursor();
}
#pragma region RESOURCE_MANAGER
CTextureBase* CGameInstance::GetOrAddTexture(const wstring& wstrKey, void* pArg)
{
	return m_pResource_Manager->GetOrAddTexture(wstrKey, pArg);
}
#pragma endregion

#pragma region DATA_REPOSITORY
HRESULT CGameInstance::Load_Folder_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath)
{
	return m_pDataRepository->Load_Folder_Json(iLevelID, eCategory, folderPath);
}
HRESULT CGameInstance::Load_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath)
{
	return m_pDataRepository->Load_File_Json(iLevelID, eCategory, folderPath);
}
HRESULT CGameInstance::Save_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath) const
{
	return m_pDataRepository->Save_File_Json(iLevelID, eCategory, folderPath);
}
CDataDocumentBase* CGameInstance::Ensure_Document(_uint iLevelID, DTO::ECategory eCategory, const path& filePath)
{
	return m_pDataRepository->Ensure_Document(iLevelID, eCategory, filePath);
}
const CDataDocumentBase* CGameInstance::Get_Document(_uint iLevelID, DTO::ECategory eCategory, const string& strFileKey)
{
	return m_pDataRepository->Get_Document(iLevelID, eCategory, strFileKey);
}
#pragma endregion

#pragma region OCTREE_MANAGER
HRESULT CGameInstance::Register_Octree(CGameObject* pGo, RENDER_CATEGORY eCategory, const BoundingBox& AABB, _bool bDynamic)
{
	OCTREE_ENTRY* pEntry = m_pOctree_Manager->Register(pGo, eCategory, AABB, bDynamic);
	if (pEntry == nullptr)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Unregister(CGameObject* pGo)
{
	m_pOctree_Manager->Unregister(pGo);
}

HRESULT CGameInstance::Ready_Octree(const OCTREE_DESC& desc)
{
	return m_pOctree_Manager->Initialize(desc);
}
#pragma endregion
void CGameInstance::Push_RenderObject(RENDER_CATEGORY eCategory, CGameObject* pGO)
{
	m_pRender_Manager->Push_RenderObject(eCategory, pGO);
}
#ifdef _DEBUG
ID3D11ShaderResourceView* CGameInstance::Get_RenderTargetSRV(ERenderTarget eTarget)
{
	return m_pRenderTarget_Manager->Get_RenderTargetSRV(eTarget);
}

inline void CGameInstance::Push_DebugComponent(CComponent* pComp)
{
	m_pRender_Manager->Push_DebugComponent(pComp);
}
SHADER_SSAOPARAM_DESC& CGameInstance::Get_SSAOParamDesc()
{
	return m_pRender_Manager->Get_SSAOParamDesc();
}
const SHADER_SSAOPARAM_DESC& CGameInstance::Get_SSAOParamDesc() const
{
	return m_pRender_Manager->Get_SSAOParamDesc();
}
HRESULT CGameInstance::Commit_SSAOParam()
{
	return m_pRender_Manager->Commit_SSAOParam();
}
SHADER_HDRPARAM_DESC& CGameInstance::Get_HDRParamDesc()
{
	return m_pRender_Manager->Get_HDRParamDesc();
}
const SHADER_HDRPARAM_DESC& CGameInstance::Get_HDRParamDesc() const
{
	return m_pRender_Manager->Get_HDRParamDesc();
}
HRESULT CGameInstance::Commit_HDRParam()
{
	return m_pRender_Manager->Commit_HDRParam();
}
SHADER_BLOOMPARAM_DESC& CGameInstance::Get_BloomParamDesc()
{
	return m_pRender_Manager->Get_BloomParamDesc();
}
const SHADER_BLOOMPARAM_DESC& CGameInstance::Get_BloomParamDesc() const
{
	return m_pRender_Manager->Get_BloomParamDesc();
}
HRESULT CGameInstance::Commit_BloomParam()
{
	return m_pRender_Manager->Commit_BloomParam();
}
SHADER_OUTLINE_DESC& CGameInstance::Get_OutlineParamDesc()
{
	return m_pRender_Manager->Get_OutlineParamDesc();
}
const SHADER_OUTLINE_DESC& CGameInstance::Get_OutlineParamDesc() const
{
	return m_pRender_Manager->Get_OutlineParamDesc();
}
HRESULT CGameInstance::Commit_OutlineParam()
{
	return m_pRender_Manager->Commit_OutlineParam();
}
HRESULT CGameInstance::Commit_AllPostParams()
{
	return m_pRender_Manager->Commit_AllPostParams();
}
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
ID3D11Buffer* CGameInstance::Get_Light_ConstantBuffer()
{
	return m_pLight_Manager->Get_Light_ConstantBuffer();
}
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
HRESULT CGameInstance::Push_DynamicLight(CLight* pLight)
{
	return m_pLight_Manager->Push_DynamicLight(pLight);
}
HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect_Tex* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
void CGameInstance::Clear_Lights()
{
	return m_pLight_Manager->Clear();
}
CLight* CGameInstance::Get_Light(LIGHT_TYPE eType, _uint iIndex)
{
	return m_pLight_Manager->Get_Light(eType, iIndex);
}
#pragma endregion

#pragma region EVENT_MANAGER
void CGameInstance::Flush_All()
{
	m_pEvent_Manager->Flush_All();
}
#pragma endregion

#pragma region FONT_MANAGER

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Request_DrawFont(FONT_DESC Desc)
{
	return m_pFont_Manager->Request_DrawFont(Desc);
}
HRESULT CGameInstance::Render_Fonts()
{
	return m_pFont_Manager->Render_Fonts();
}
#pragma endregion

#pragma region EFFECT_MANAGER
void CGameInstance::Spawn_PoolEffect(CEffectHandler* handler, const std::string& UniqueEffectName, const std::string& strTag, const Matrix& matWorld, _float fDuration, _uint bIsLocal, _uint iFlag, const Matrix* pTargetBone, const Matrix* pTargetTransMatrix)
{
	m_pEffect_Manager->Spawn_PoolEffect(handler, UniqueEffectName, strTag, matWorld, fDuration, bIsLocal, iFlag, pTargetBone, pTargetTransMatrix);
}

void CGameInstance::Spawn_PoolEffect(const std::string& strTag, const Matrix& matWorld, _float fDuration, _uint bIsLocal, _uint iFlag, const Matrix* pTargetBone, const Matrix* pTargetTransMatrix)
{
	m_pEffect_Manager->Spawn_PoolEffect(strTag, matWorld, fDuration, bIsLocal, iFlag, pTargetBone, pTargetTransMatrix);
}

void CGameInstance::Push_CollidedData(const COLLIDED_DESC& desc)
{
	m_pJudgementSystem->Push_CollidedData(desc);
}

#pragma endregion


void CGameInstance::Destroy_Engine()
{
	Safe_Release(m_pJudgementSystem);
	Safe_Release(m_pFrustrum);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pTimeScale_Manager);
	Safe_Release(m_pDataRepository);
	Safe_Release(m_pRender_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pOctree_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pObjectPool_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pGameData_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pEventBus_Manager);
	Safe_Release(m_pShaderAsset_Manager);
	Safe_Release(m_pResource_Manager);
	Safe_Release(m_pPhysics_Module);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::GetInstance()->DestroyInstance();
}

#pragma region FRUSTRUM
void CGameInstance::Ready_Frustrum()
{
	m_pFrustrum->Initialize();
}
_float CGameInstance::Get_FrustrumMidStart() const
{
	return m_pFrustrum->Get_MidStart();
}
_float CGameInstance::Get_FrustrumFarStart() const
{
	return m_pFrustrum->Get_FarStart();
}
void CGameInstance::Resize_SplitFrustrum(const _float fMidStart, const _float fFarStart)
{
	m_pFrustrum->Resize_SplitFrustrum(fMidStart, fFarStart);
}
EFrustrumTier CGameInstance::Classify_BySplitFrustrum(const BoundingBox& AABB)
{
	return m_pFrustrum->Classify_BySplitFrustrum(AABB);
}
EFrustrumTier CGameInstance::Classify_BySplitFrustrum(const BoundingSphere& Sphere)
{
	return m_pFrustrum->Classify_BySplitFrustrum(Sphere);
}
BoundingFrustum* CGameInstance::Get_BoundingFrustrum_Local()
{
	return m_pFrustrum->Get_BoundingFrustrum_Local();
}
BoundingFrustum* CGameInstance::Get_BoundingFrustrum_World()
{
	return m_pFrustrum->Get_BoundingFrustrum_World();
}
#pragma endregion

#pragma region RENDERTARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(ERenderTarget eTarget, const CRenderTarget::RENDERTARGET_DESC* pDesc)
{
	return m_pRenderTarget_Manager->Add_RenderTarget(eTarget, pDesc);
}

HRESULT CGameInstance::Add_MRT(EMRTLayer eMRTLayer, ERenderTarget eTarget)
{
	return m_pRenderTarget_Manager->Add_MRT(eMRTLayer, eTarget);
}

HRESULT CGameInstance::Begin_MRT(EMRTLayer eMRTLayer, _bool bClear, _bool bUseDSV)
{
	return m_pRenderTarget_Manager->Begin_MRT(eMRTLayer, bClear, bUseDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pRenderTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(ERenderTarget eTarget, CShader* pShader)
{
	return m_pRenderTarget_Manager->Bind_ShaderResource(eTarget, pShader);
}

HRESULT CGameInstance::Copy_SceneHDRResource(ERenderTarget eTarget)
{
	return m_pRenderTarget_Manager->Copy_SceneHDRResource(eTarget);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(ERenderTarget eTarget, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pRenderTarget_Manager->Ready_Debug(eTarget, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Debug_RT_Render(EMRTLayer eMRTLayer, CShader* pShader, CVIBuffer_Rect_Tex* pVIBuffer)
{
	return m_pRenderTarget_Manager->Render(eMRTLayer, pShader, pVIBuffer);
}
#endif
#pragma endregion

#pragma region PHYSICS_MODULE
void CGameInstance::StepPhysics(_float fTimeDelta)
{
	m_pPhysics_Module->StepPhysics(fTimeDelta);
}

void CGameInstance::AddActor(PxRigidActor* actor)
{
	m_pPhysics_Module->AddActor(actor);
}

void CGameInstance::ClearPhysics()
{
	m_pPhysics_Module->ClearPhysics();
}

PxTransform CGameInstance::XMMatrixToPxTransform(Matrix mat)
{
	return m_pPhysics_Module->XMMatrixToPxTransform(mat);
}

Matrix CGameInstance::PxTransformToXMMatrix(PxTransform pxTransform)
{
	return m_pPhysics_Module->PxTransformToXMMatrix(pxTransform);
}

_bool CGameInstance::Execute_Overlap(PxGeometry& shape, PxTransform& transform, OUT PxOverlapBuffer& hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback)
{
	return m_pPhysics_Module->Execute_Overlap(shape, transform, hit, filterData, filterCallback);
}

CPhysics_QueryFilterCallback* CGameInstance::GetQueryFilterCallback()
{
	return m_pPhysics_Module->GetQueryFilterCallback();
}

void CGameInstance::SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes)
{
	m_pPhysics_Module->SerializeStaticMesh(path, meshes);
}

PxCollection* CGameInstance::DeserializeStaticMesh(std::filesystem::path path)
{
	return m_pPhysics_Module->DeserializeStaticMesh(path);
}

void CGameInstance::SerializeConvexMesh(std::filesystem::path path, vector<PxConvexMesh*> meshes)
{
	m_pPhysics_Module->SerializeConvexMesh(path, meshes);
}

PxCollection* CGameInstance::SerializeConvexMesh(std::filesystem::path path)
{
	return m_pPhysics_Module->SerializeConvexMesh(path);
}

vector<PxShape*> CGameInstance::GetShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pPhysics_Module->GetShape(pDesc);
}

vector<PxShape*> CGameInstance::GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pPhysics_Module->GetMeshShape(pDesc);
}

vector<PxShape*> CGameInstance::CopyShapes(vector<PxShape*>& shapes)
{
	return m_pPhysics_Module->CopyShapes(shapes);
}

vector<PxRigidActor*> CGameInstance::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	return m_pPhysics_Module->GetActor(rigidBodyDesc, colliderDesc, shapes);
}

PxController* CGameInstance::GetController(PHYSICSCCT_DESC* pDesc)
{
	return m_pPhysics_Module->GetController(pDesc);
}

CPhysics_CCTFilterCallback* CGameInstance::GetCCTFilterCallback()
{
	return m_pPhysics_Module->GetCCTFilterCallback();
}

void CGameInstance::RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag)
{
	m_pPhysics_Module->RegisterPhysicsMesh(levelIndex, prototypeTag);
}

PxQuat CGameInstance::GetPureRotation(const Matrix& mat)
{
	return m_pPhysics_Module->GetPureRotation(mat);
}

PxVec3 CGameInstance::GetPureScale(const Matrix& mat)
{
	return m_pPhysics_Module->GetPureScale(mat);
}

void CGameInstance::Overlap_EventCallback(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc)
{
	return m_pPhysics_Module->Overlap_EventCallback(pOwner, vOverlapPoint, pOverlapHit, event, hitboxDesc);
}

_bool CGameInstance::RayCast(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall)
{
	return m_pPhysics_Module->RayCast(vWorldPos, vDir, fMaxDist, pFilterCall);
}

#ifdef _DEBUG
void CGameInstance::Physics_Render(PxRigidActor* pActor, XMVECTOR color)
{
	m_pPhysics_Module->Render(pActor, color);
}
void CGameInstance::Physics_Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color)
{
	m_pPhysics_Module->Render(geom, transform, color);
}
#endif
#pragma endregion

#pragma region GAMEDATA_MANAGER


#pragma region Texture Splating
HRESULT CGameInstance::GameDataManager_Load_TextureSplatingInfoData()
{
	return m_pGameData_Manager->Load_TextureSplatingInfoData();
}
HRESULT CGameInstance::GameDataManager_Bind_SplatingTextureInfo(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	return m_pGameData_Manager->Bind_SplatingTextureInfo(pBindShader, wstrTextureSplatingInfoDataName);
}
#pragma endregion

#pragma region CameraCinematicSequence
HRESULT CGameInstance::GameDataManager_Load_CameraCinematicSequence()
{
	return m_pGameData_Manager->Load_CameraCinematicSequence();
}

HRESULT CGameInstance::GameDataManager_Save_CameraCinematicSequence()
{
	return m_pGameData_Manager->Save_CameraCinematicSequence();
}

HRESULT CGameInstance::GameDataManager_Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT Camera_Cinematic_Sequence* pOutCamCinematicSequence)
{
	return m_pGameData_Manager->Load_CameraCinematicSequence(wstrFindKey , pOutCamCinematicSequence);
}

HRESULT CGameInstance::GameDataManager_Save_CameraCinematicSequence(const wstring& wstrFindKey, const Camera_Cinematic_Sequence* pSaveCamCinematicSequence)
{
	return m_pGameData_Manager->Save_CameraCinematicSequence(wstrFindKey, pSaveCamCinematicSequence);
}
vector<std::string> CGameInstance::GameDataManager_Get_CameraCinematicSequenceNames() const
{
	return m_pGameData_Manager->Get_CameraCinematicSequenceNames();
}
#pragma endregion

#pragma region Attack Preset
const DTO::TAttackPreset_Data* CGameInstance::Find_AttackPrseet(_uint iPresetKey) const
{
	return m_pGameData_Manager->Find_AttackPrseet(iPresetKey);
}
const DTO::TAttackPreset_Data* CGameInstance::Find_AttackPresetByTag(const string& strTag) const
{
	return m_pGameData_Manager->Find_AttackPresetByTag(strTag);
}
_uint CGameInstance::Get_AttackPresetIdByTag(const string& strTag) const
{
	return m_pGameData_Manager->Get_AttackPresetIdByTag(strTag);
}
HRESULT CGameInstance::Upsert_AttackPresetData(const DTO::TAttackPreset_Data& inData)
{
	return m_pGameData_Manager->Upsert_AttackPresetData(inData);
}
const unordered_map<_uint, DTO::TAttackPreset_Data>& CGameInstance::Get_AttackPresetsData_ForDebug() const
{
	return m_pGameData_Manager->Get_AttackPresetsData_ForDebug();
}
#pragma endregion

#pragma endregion
void CGameInstance::Free()
{
	Safe_Release(m_pJudgementSystem);
	Safe_Release(m_pFrustrum);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pTimeScale_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pDataRepository);
	Safe_Release(m_pRender_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pOctree_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pObjectPool_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pGameData_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pEventBus_Manager);
	Safe_Release(m_pShaderAsset_Manager);
	Safe_Release(m_pResource_Manager);
	Safe_Release(m_pPhysics_Module);
	Safe_Release(m_pGraphic_Device);
	Super::Free();
}

