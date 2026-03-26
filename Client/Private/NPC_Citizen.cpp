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
{
}

CNPC_Citizen::CNPC_Citizen(const CNPC_Citizen& rhs)
	: CNPC_Base(rhs)
	, m_tCitizenData{rhs.m_tCitizenData }
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
	/* Transform 积己 */
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	CNPC_Citizen::NPC_CITIZEN_DESC* pDesc = static_cast<CNPC_Citizen::NPC_CITIZEN_DESC*>(pArg);

	m_tCitizenData = pDesc->tCitizenData;

	/* Citizen阑 困茄 part积己 */
	if (FAILED(Ready_CitizenParts(pDesc)))
		return E_FAIL;

	/* CCT 积己 */
	if (FAILED(CNPC_Base::Ready_CCT(pArg)))
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


	m_vecPartObjects.resize(1 + (_uint)DTO::CITIZEN_PARTTYPE::END);
	if (FAILED(Add_Part(ENUM_TO_UINT(CNPC_Citizen::Part::BODY), ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_Body_Prototype_Tag, &tBodyDesc)))
		return E_FAIL;
	

	/* Decor Part */

	CNPC_Citizen_DecoPart::NPC_CITIZEN_DECOPARTDESC tDecoDesc{};
	tDecoDesc.pBoneSocket	= &m_vecPartObjects[ENUM_TO_UINT(CNPC_Citizen::Part::BODY)]->Get_Component<CModel>()->Get_Bone("Jiao")->Get_CombinedTransformMatrix();
	tDecoDesc.pMatParent	= Get_Component<CTransform>()->Get_WorldMatrixPtr();
	tDecoDesc.iLevelIndex	= pDesc->iLevelIndex;
	_uint i = 0;
	for (auto& PartData : pDesc->tCitizenData.arrayPartDatas)
	{
		tDecoDesc.tTintColor = PartData.vColor;
		tDecoDesc.wstrPartModelFolderName = Engine_Utils::ToWString(PartData.strName);

		if (FAILED(Add_Part(i + 1, ENUM_TO_UINT(ELevelType::STATIC), g_wszNPC_Citizen_DecoPart_Prototype_Tag, &tDecoDesc)))
			return E_FAIL;
		++i;
	}


	return S_OK;
}

HRESULT CNPC_Citizen::Ready_Component(CNPC_Citizen::NPC_CITIZEN_DESC* pArgs)
{



	return S_OK;
}

HRESULT CNPC_Citizen::Awake(const _uint iCurrentLevelID)
{
	Set_Awake(true);

	for (CPartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Awake(iCurrentLevelID);
	}

	Get_Component<CPhysicsCCT>()->Ready_Position();



	return S_OK;
}

void CNPC_Citizen::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Citizen::Update(const _float fTimeDelta)
{
	//Super::Update(fTimeDelta);
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
	Super::QuestEnter();
}

void CNPC_Citizen::QuestExit()
{
	Super::QuestExit();
}

void CNPC_Citizen::Interact()
{
	Super::Interact();
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
	desc.MAccelRate = { 0.f, 10.f };
	desc.MDeAccelRate = { 0.f, 10.f };
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
}
