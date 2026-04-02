#include "pch.h"
#include "TriggerBox_MonsterSpawner.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"

//=================
// Builder
//=================
#include "Builder_Map.h"

//=================
// Monster
//=================
#include "Monster_Base.h"
#include "Monster_Body_Base.h"
#include "MonsterState_Factory.h"
#include "MonsterControlContext.h"
#include "MonsterActionState.h"

#include "Monster_Dog.h"
#include "Monster_Boomer.h"
#include "Monster_Fly.h"

CTriggerBox_MonsterSpawner::CTriggerBox_MonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTriggerBox(pDevice ,pContext) 
	, m_vecMonsterSpawnData{}
{
    m_eTriggerBoxType = CTriggerBox::Type::MONSTER_SPAWNER;
}

CTriggerBox_MonsterSpawner::CTriggerBox_MonsterSpawner(const CTriggerBox_MonsterSpawner& rhs)
    : CTriggerBox(rhs)
    , m_vecMonsterSpawnData{rhs.m_vecMonsterSpawnData }
{
}

HRESULT CTriggerBox_MonsterSpawner::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    Set_Object_Enum_Tag(OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT);

    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

	TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc = static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(pArg);

	m_vecMonsterSpawnData = pDesc->vecMonsterSpawnData;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_SpawnPool(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Ready_Component(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc)
{
    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Ready_SpawnPool(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc)
{
    _uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
    
    map<DTO::EMakeMonsterType, _int> poolAggregate;

    for (auto& tData : m_vecMonsterSpawnData)
    {
        poolAggregate[tData.eMakeMonsterType]++;
        //auto item = poolAggregate.find(tData.eMakeMonsterType);
        //(*item).second += 1;
    }

    for (auto& item : poolAggregate)
    {
        if (FAILED(Register_Pool(pDesc->iLevelIndex, iFindPrototypeIndex, item.first, item.second)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Register_Pool(_uint iLevelId, _uint iFindPrototypeIndex, DTO::EMakeMonsterType eMakeMonsterType, _int numPool)
{
    switch (eMakeMonsterType)
    {
    case DTO::EMakeMonsterType::Dog:
    {
        auto desc = CMonster_Dog::Get_PreSetDesc(iLevelId);
        m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Dog, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Dog_Prototype_Tag, &desc, numPool + 80);
    }
    break;
    case DTO::EMakeMonsterType::Boomer:
    {
        auto desc = CMonster_Boomer::Get_PreSetDesc(iLevelId);
        m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Boomer, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Boomer_Prototype_Tag, &desc, numPool + 60);
    }
        break;
    case DTO::EMakeMonsterType::Shooter:
        break;
    case DTO::EMakeMonsterType::Fly:
    {
        auto desc = CMonster_Fly::Get_PreSetDesc(iLevelId);
        m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Fly, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Fly_Prototype_Tag, &desc, numPool + 60);
    }
        break;
    case DTO::EMakeMonsterType::Xibi:
        break;
    case DTO::EMakeMonsterType::END:
        break;
    default:
        break;
    }

    return S_OK;
}


HRESULT CTriggerBox_MonsterSpawner::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CTriggerBox_MonsterSpawner::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_MonsterSpawner::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CTriggerBox_MonsterSpawner::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CTriggerBox_MonsterSpawner::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_MonsterSpawner::Render()
{
    return S_OK;
}

void CTriggerBox_MonsterSpawner::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);

}

void CTriggerBox_MonsterSpawner::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CTriggerBox_MonsterSpawner::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    if (Super::IsEnabled() == false)
        return;

    Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_MonsterSpawner::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    m_iOverlapCount++;

    if (Super::IsEnabled() == false || m_bLockedEnter == true)
        return;

    Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

    if (m_isTriggerEventPlay == true) return;


	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		if (FAILED(SpawnMonster()))
		{
			MSG_BOX("Mosnter Spawner 작동 오류");
			return;
		}
	}

    if (Super::m_bHasQuest)
    {
        m_bLockedEnter = true;
        CallQuestEvent(Get_Object_Enum_Tag(), 1);
    }


    m_isTriggerEventPlay = true;
}

void CTriggerBox_MonsterSpawner::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    m_iOverlapCount--;

    if (Super::IsEnabled() == false || m_bLockedExit == true)
        return;

    if (m_iOverlapCount > 0)
        return;

    m_iOverlapCount = 0;

    Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

    if (Super::m_bHasQuest)
    {
        SetEnable(false);
        Super::m_bLockedExit = true;
        CallQuestEvent(Get_Object_Enum_Tag(), 1);
    }
}

void CTriggerBox_MonsterSpawner::QuestEnter()
{
    Super::QuestEnter();
}

void CTriggerBox_MonsterSpawner::QuestExit()
{
    Super::QuestExit();
}

HRESULT CTriggerBox_MonsterSpawner::SpawnMonster()
{
	_uint iCurLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pResult = { nullptr };

	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};

	CTransform::TRANSFORM_DESC tTransformDesc = {};

    wstring poolTag = {};

	for (auto& tData : m_vecMonsterSpawnData)
	{
		tTransformDesc.ScaleMatrix		= Matrix::CreateScale(tData.vScale);
        tTransformDesc.TranslationMatrix = Matrix::CreateTranslation(tData.vPosition);
        tTransformDesc.RotationMatrix	= Matrix::CreateFromYawPitchRoll(
			XMConvertToRadians(tData.vPitchYawRoll.y), XMConvertToRadians(tData.vPitchYawRoll.x), XMConvertToRadians(tData.vPitchYawRoll.z)
		);

        //if (FAILED(CMonster_Base::Create_Mosnter(CBuilder_Map::Change_MakeMonsterType_To_MonsterType(tData.eMakeMonsterType) , iFindPrototypeIndex , iCurLevelIndex  , &tTransformDesc)))
        //    return E_FAIL;
        
        switch (tData.eMakeMonsterType)
        {
        case DTO::EMakeMonsterType::Dog:
            poolTag = g_wszPool_Monster_Dog;
        break;
        case DTO::EMakeMonsterType::Boomer:
            poolTag = g_wszPool_Monster_Boomer;
        break;
        case DTO::EMakeMonsterType::Fly:
            poolTag = g_wszPool_Monster_Fly;
        break;

        // todo
        case DTO::EMakeMonsterType::Shooter:
        case DTO::EMakeMonsterType::Xibi:
        default:
            continue;
        }

        m_pGameInstance->Request_AddObject(iCurLevelIndex, poolTag, iCurLevelIndex, nullptr,
            [this, tTransformDesc](CGameObject* p)
            {
                auto* pMonsterObject = static_cast<CMonster_Base*>(p);
                if (nullptr == pMonsterObject)
                    return;
                pMonsterObject->SetSpawnPos(tTransformDesc);
            });
	}

    return S_OK;
}

CTriggerBox_MonsterSpawner* CTriggerBox_MonsterSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriggerBox_MonsterSpawner* pTriggerBox = new CTriggerBox_MonsterSpawner(pDevice ,pContext);

    if (FAILED(pTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Monster Spawner Is Failed To Craete ");
        return nullptr;
    }

    return pTriggerBox;
}

CGameObject* CTriggerBox_MonsterSpawner::Clone(void* pArg)
{
    CTriggerBox_MonsterSpawner* pTriggerBox = new CTriggerBox_MonsterSpawner(*this);

    if (FAILED(pTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Monster Spawner Is Failed To Clone ");
        return nullptr;
    }

    return pTriggerBox;
}

void CTriggerBox_MonsterSpawner::Free()
{
    Super::Free();

    return;
}

