#include "pch.h"
#include "NPC_Citizen.h"
#include "NPC_Citizen_Body.h"
#include "NPC_Citizen_DecoPart.h"
#include "Bone.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"


CNPC_Citizen::CNPC_Citizen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CNPC_Base(pDevice,pDeviceContext)
	, m_tCitizenData{}
	, m_isWalking{false}
	, m_pWayPointData{nullptr}
	, m_iCurrentFrameIndex{0}
	, m_fDeltaTime{0}
	, m_tWalkRunAnimIndex{}
	, m_isArrive{false}
	, m_vLastPosition{}
	, m_fDisloveRange{}
	, m_isOnReachWayPointtDissloveStart{ false }
{
}

CNPC_Citizen::CNPC_Citizen(const CNPC_Citizen& rhs)
	: CNPC_Base(rhs)
	, m_tCitizenData{rhs.m_tCitizenData }
	, m_isWalking{rhs.m_isWalking }
	, m_pWayPointData{rhs.m_pWayPointData }
	, m_iCurrentFrameIndex{rhs.m_iCurrentFrameIndex }
	, m_fDeltaTime{rhs.m_fDeltaTime }
	, m_tWalkRunAnimIndex{rhs.m_tWalkRunAnimIndex }
	, m_isArrive{rhs.m_isArrive }
	, m_vLastPosition{rhs.m_vLastPosition }
	, m_fDisloveRange{rhs.m_fDisloveRange }
	, m_isOnReachWayPointtDissloveStart{rhs.m_isOnReachWayPointtDissloveStart }
{
}

HRESULT CNPC_Citizen::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Citizen::Initialize(void* pArg)
{
	/* Transform 생성 */
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	CNPC_Citizen::NPC_CITIZEN_DESC* pDesc = static_cast<CNPC_Citizen::NPC_CITIZEN_DESC*>(pArg);

	/* Preset Data 가 알아서 Count가 증가되고 알아서 0~부터 준다 */
	if (true == (m_isWalking = pDesc->isWalking))
		pDesc->tCitizenData = m_tCitizenData = DTO::CitizenPresetData::Get_Preset_ForRandom();
	else
		m_tCitizenData = pDesc->tCitizenData;


	m_tWalkRunAnimIndex = pDesc->tCitizenData.tWalkRunAnimIndex;	/* Walk , Run Anim Index 받아오기 */

	/* UI Text Offset 조정 */
	m_vUITextOffset = DTO::CitizenUITextData::Get_CitizenTextOffset(Engine_Utils::ToWString(pDesc->tCitizenData.strModelName));

	/* CCT 생성 */

	Set_Name(pDesc->wstrNPCName);
	m_wstrContents = pDesc->wstrNPCText;
	m_wstrSoundTag = pDesc->wstrSoundTag;

	if (FAILED(CNPC_Base::Ready_CCT(pArg)))
		return E_FAIL;

	/* Citizen을 위한 part생성 */
	if (FAILED(Ready_CitizenParts(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Citizen::Ready_CitizenParts(CNPC_Citizen::NPC_CITIZEN_DESC* pDesc)
{
	/* Body Part */
	CNPC_Citizen_Body::NPC_CITIZEN_BODY tBodyDesc{};
	tBodyDesc.iLevelIndex = pDesc->iLevelIndex;
	tBodyDesc.pMatParent = this->Get_Component<CTransform>()->Get_WorldMatrixPtr();
	tBodyDesc.wstrModelPrototypeTag = Engine_Utils::ToWString(pDesc->tCitizenData.strModelName);
	tBodyDesc.strLoopAnimName = pDesc->tCitizenData.strLoopAnimationName;
	tBodyDesc.tRGBColorData			= pDesc->tCitizenData.tClothRGBColor;
	tBodyDesc.arrayAtlasDatas		= pDesc->tCitizenData.arrayNpcAtlasData;

	m_vecPartObjects.resize(1 + (_uint)DTO::CITIZEN_PARTTYPE::END);
	if (FAILED(Add_Part(ENUM_TO_UINT(CNPC_Citizen::Part::Body), ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_Body_Prototype_Tag, &tBodyDesc)))
		return E_FAIL;

	/* UI Text Offset 조정 */
	m_vUITextOffset = DTO::CitizenUITextData::Get_CitizenTextOffset(Engine_Utils::ToWString(pDesc->tCitizenData.strModelName));

	/* Decor Part */

	CNPC_Citizen_DecoPart::NPC_CITIZEN_DECOPARTDESC tDecoDesc{};
	tDecoDesc.pBoneSocket	= &m_vecPartObjects[ENUM_TO_UINT(CNPC_Citizen::Part::Body)]->Get_Component<CModel>()->Get_Bone("Jiao")->Get_CombinedTransformMatrix();
	tDecoDesc.pMatParent	= Get_Component<CTransform>()->Get_WorldMatrixPtr();
	tDecoDesc.iLevelIndex	= pDesc->iLevelIndex;

	_uint i = 0;
	for (auto& PartData : pDesc->tCitizenData.arrayPartDatas)
	{
		if (PartData.strName.empty())
			continue;
		tDecoDesc.tTintColor = PartData.vColor;
		tDecoDesc.wstrPartModelFolderName = Engine_Utils::ToWString(PartData.strName);
	
		/* Body 다음으로 타입별로 잘들어간다 */
		if (FAILED(Add_Part( i + 1 , ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_DecoPart_Prototype_Tag, &tDecoDesc)))
			return E_FAIL;
		++i;
	}
	return S_OK;
}

HRESULT CNPC_Citizen::Ready_Component(CNPC_Citizen::NPC_CITIZEN_DESC* pArgs)
{
	/* Action State 생성 */



	return S_OK;
}

HRESULT CNPC_Citizen::Change_WalkCitizenModel(const DTO::CITIZEN_DATA& tData)
{
	/* 처음 만들어질떄 SetPosition을 해주기 때문에 */
	for(_uint i = 0 ; i < ENUM_TO_UINT(CNPC_Citizen::Part::END) ; ++i )
		Remove_Part(i);

	_uint iCurLevel = m_pGameInstance->Get_CurrentLevelIndex();
	/* Body Part */
	static CNPC_Citizen_Body::NPC_CITIZEN_BODY tBodyDesc{};
	tBodyDesc.iLevelIndex = iCurLevel;
	tBodyDesc.pMatParent = this->Get_Component<CTransform>()->Get_WorldMatrixPtr();
	tBodyDesc.wstrModelPrototypeTag = Engine_Utils::ToWString(tData.strModelName);
	tBodyDesc.iLoopAnimIndex = DTO::CitizenWalkRunAnimIndexData::Get_CitizenWalkRunAnimIndex(tData.strModelName).iRunAnimIndex; /* Hash 값으로 찾아옴 */
	tBodyDesc.tRGBColorData = tData.tClothRGBColor;
	tBodyDesc.arrayAtlasDatas = tData.arrayNpcAtlasData;

	m_vecPartObjects.resize(1 + (_uint)DTO::CITIZEN_PARTTYPE::END);
	if (FAILED(Add_Part(ENUM_TO_UINT(CNPC_Citizen::Part::Body), ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_Body_Prototype_Tag, &tBodyDesc)))
		return E_FAIL;

	/* Decor Part */

	static CNPC_Citizen_DecoPart::NPC_CITIZEN_DECOPARTDESC tDecoDesc{};
	tDecoDesc.pBoneSocket = &m_vecPartObjects[ENUM_TO_UINT(CNPC_Citizen::Part::Body)]->Get_Component<CModel>()->Get_Bone("Jiao")->Get_CombinedTransformMatrix();
	tDecoDesc.pMatParent = Get_Component<CTransform>()->Get_WorldMatrixPtr();
	tDecoDesc.iLevelIndex = iCurLevel;

	_uint i = 0;
	for (auto& PartData : tData.arrayPartDatas)
	{
		if (PartData.strName.empty())
			continue;
		tDecoDesc.tTintColor = PartData.vColor;
		tDecoDesc.wstrPartModelFolderName = Engine_Utils::ToWString(PartData.strName);

		/* Body 다음으로 타입별로 잘들어간다 */
		if (FAILED(Add_Part(i + 1, ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_DecoPart_Prototype_Tag, &tDecoDesc)))
			return E_FAIL;
		++i;
	}

	return S_OK;
}

HRESULT CNPC_Citizen::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}


HRESULT CNPC_Citizen::Spawn_FromPool(void* pArg)
{
	Super::Spawn_FromPool(pArg);

	Reset_MoveData();

	CNPC_Citizen::NPC_CITIZEN_POOL_DESC* pDesc{ static_cast<CNPC_Citizen::NPC_CITIZEN_POOL_DESC*>(pArg) };
	if (pDesc == nullptr) return E_FAIL;
	if (pDesc->tMoveData.pWayPointData == nullptr) return E_FAIL;

	m_pWayPointData = pDesc->tMoveData.pWayPointData;
	if (m_pWayPointData == nullptr) return E_FAIL;

	_uint iSize = (_uint)m_pWayPointData->vecPosition.size();
	m_vLastPosition = m_pWayPointData->vecPosition[iSize - 1];

	/* 지금 이미 생성된 모델 그대로 유지 */
	if (m_pWayPointData->vecPosition.empty()) return E_FAIL;

	CNPC_Citizen_Body* pBody = Get_Part<CNPC_Citizen_Body>(ENUM_TO_UINT(CNPC_Citizen::Part::Body));
	if (pBody == nullptr) return E_FAIL;

	_uint iChangeAnimIndex{ 0 };
	pDesc->tMoveData.eMoveType == DTO::CITIZEN_MOVE_TYPE::RUN ? iChangeAnimIndex = m_tWalkRunAnimIndex.iRunAnimIndex : iChangeAnimIndex = m_tWalkRunAnimIndex.iWalkAnimIndex;
	pDesc->tMoveData.eMoveType == DTO::CITIZEN_MOVE_TYPE::RUN ? m_fDisloveRange = 5.f : m_fDisloveRange = 3.f;

	pBody->Change_Animation(iChangeAnimIndex);

	CPhysicsCCT* pCCT = Get_Component<CPhysicsCCT>();
	if (pCCT == nullptr) return E_FAIL;
	pCCT->SetFootPosition(m_pWayPointData->vecPosition[0]);
	pCCT->Ready_Position();
	pCCT->Set_Speed(pDesc->tMoveData.fSpeed);

	CTransform* pTs = Get_Component<CTransform>();
	if (pTs == nullptr) return  E_FAIL;
	pTs->Set_Info(TRANSFORM_INFO_STATE::POS, m_pWayPointData->vecPosition[0]);


	/* NPC 이름 Text변경 */
	const wstring& wstrModelName = Engine_Utils::ToWString(this->m_tCitizenData.strModelName);
	m_strName = DTO::CitizenUITextData::Get_RandomCitizenName(wstrModelName);
	m_wstrContents = Engine_Utils::ToWString(DTO::CitizenUITextData::Get_RandomCitizenText(wstrModelName));


	static_cast<CNPC_Citizen_Body*>(m_vecPartObjects[(_uint)Part::Body])->Ready_DissolveEffect_Setting();

	return S_OK;
}

void CNPC_Citizen::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Citizen::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (!m_pWayPointData)
		return;

	if (!m_pWayPointData)
		return;

	const auto& vecPos = m_pWayPointData->vecPosition;
	int iSize = (_int)vecPos.size();

	if (iSize <= 1 || m_iCurrentFrameIndex >= (_uint)iSize)
	{
		m_isArrive = true;
		return;
	}

	CTransform* pTs = Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_Component<CPhysicsCCT>();


	Vec3 vCurPos = pTs->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPos = vecPos[m_iCurrentFrameIndex];


	Vec3 vDir = vTargetPos - vCurPos;
	vDir.y = 0.f; 

	float fDistance = vDir.Length();


	float fArrivalRadius = 0.5f;
	if (fDistance <= fArrivalRadius)
	{
		m_iCurrentFrameIndex++;
		return; 
	}
	if (fDistance > 0.0001f)
	{
		vDir.Normalize();
		pTs->Turn_WorldYAxis(-vDir, fTimeDelta);
	}




	if (pCCT)
	{
		pCCT->SetInputDir(vDir);

		pCCT->Update(fTimeDelta);
	}


	if (!m_isOnReachWayPointtDissloveStart)
	{

		float fLastPosDistance = Vec3::Distance(m_vLastPosition, vCurPos);
		if (fLastPosDistance < m_fDisloveRange)
		{
			static_cast<CNPC_Citizen_Body*>(m_vecPartObjects[(_uint)Part::Body])->DissolveStarts();

			m_isOnReachWayPointtDissloveStart = true;
		}
	}

	return;
}

void CNPC_Citizen::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Citizen::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CNPC_Citizen::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Citizen::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);

}

void CNPC_Citizen::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther,tHitInfo);
}

void CNPC_Citizen::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

void CNPC_Citizen::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther,tHitInfo);

}

void CNPC_Citizen::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

_bool CNPC_Citizen::On_Hit(const HIT_DESC& hitDesc)
{
	return Super::On_Hit(hitDesc);
}

void CNPC_Citizen::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);
}

void CNPC_Citizen::QuestEnter()
{
	//Super::QuestEnter();
}

void CNPC_Citizen::QuestExit()
{
	//Super::QuestExit();
}

void CNPC_Citizen::Interact()
{
	//Super::Interact();
}

void CNPC_Citizen::Reset_WayPoint()
{
	m_fDeltaTime = 0.f;
	m_iCurrentFrameIndex = 0;
	m_pWayPointData = nullptr;
}

void CNPC_Citizen::Reset_MoveData()
{
	m_isOnReachWayPointtDissloveStart = false;
	m_fDisloveRange = 0.f;
	m_vLastPosition = {0.f,0.f,0.f};
	m_fDeltaTime = 0.f;
	m_iCurrentFrameIndex = 0;
	m_pWayPointData = nullptr;
	m_isArrive = false;
}

CNPC_Base::NPC_DESC CNPC_Citizen::Get_PreSetDesc(_uint iLevelId)
{
	CNPC_Base::NPC_DESC tDesc{};

	tDesc.iLevelIndex = iLevelId;
#pragma region CCT Desc
	PHYSICSCCT_DESC desc;
	desc.pOwner = nullptr;
	desc.bIsPlayer = false;
	desc.eType = EPhysicsCCTType::CAPSULE;
	desc.pOwnerMatrix = nullptr;
	desc.fRadius = 0.3f;
	desc.fHeight = 0.7f;
	desc.vExtens = { 2.f, 2.f, 2.f };

	desc.fContactOffset = 0.01f;
	desc.fStepOffset = 0.2f;
	desc.fSlopeLimit = 0.7f;

	desc.vLocalOffset = {};
	desc.vWorldOffset = {};

	desc.bIsHover = { false };
	desc.fHoverOffset = { 1.f };

	PHYSICSMATERIAL_DESC mtrlDesc{};
	mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
	desc.tMaterial = mtrlDesc;

	desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::NPC;
	desc.iFilterMask =
		PHYSICSFILTERGROUP::Enum::NPC
		| PHYSICSFILTERGROUP::Enum::PLAYER
		| PHYSICSFILTERGROUP::Enum::MONSTER
		| PHYSICSFILTERGROUP::Enum::MAP
		| PHYSICSFILTERGROUP::Enum::DETECT_INTERACT;

	desc.bGravity = { true };
	desc.fGravity = { -35.f };
	desc.MSpeed = { 0.f, 4.5f };
	desc.MAccelRate = { 0.f , 20.f };		/* 최고속도까지 가는 시간 비율 */
	desc.MDeAccelRate = { 0.f, 10.f };		/* 위의 가중치를 통해 최고속도까지 도달 하는 비율 */
#pragma endregion
	tDesc.tCCTDesc = desc;


	return tDesc;
}

CNPC_Citizen* CNPC_Citizen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPC_Citizen* pNPC = new CNPC_Citizen(pDevice, pDeviceContext);

	if (FAILED(pNPC->Initialize_Prototype()))
	{
		Safe_Release(pNPC);
		MSG_BOX( "NPC Citizen is Failed To Create");
		return nullptr;
	}

	return pNPC;
}

CGameObject* CNPC_Citizen::Clone(void* pArg)
{
	CNPC_Citizen* pNPC = new CNPC_Citizen(*this);

	if (FAILED(pNPC->Initialize(pArg)))
	{
		Safe_Release(pNPC);
		MSG_BOX("NPC Citizen is Failed To Clone");
		return nullptr;
	}

	return pNPC;
}

void CNPC_Citizen::Free()
{
	Super::Free();
	m_pWayPointData = nullptr;
}
