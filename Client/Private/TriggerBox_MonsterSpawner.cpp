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

    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Ready_Component(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc)
{


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
    Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

void CTriggerBox_MonsterSpawner::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);


	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		if (FAILED(SpawnMonster()))
		{
			MSG_BOX("Mosnter Spawner 작동 오류");
			return;
		}
	}

}

void CTriggerBox_MonsterSpawner::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

HRESULT CTriggerBox_MonsterSpawner::SpawnMonster()
{
	_uint iCurLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pResult = { nullptr };

	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};

	CTransform::TRANSFORM_DESC tTransformDesc = {};

	for (auto& tData : m_vecMonsterSpawnData)
	{
		tTransformDesc.ScaleMatrix		= Matrix::CreateScale(tData.vScale);
        tTransformDesc.TranslationMatrix = Matrix::CreateTranslation(tData.vPosition);
        tTransformDesc.RotationMatrix	= Matrix::CreateFromYawPitchRoll(
			XMConvertToRadians(tData.vPitchYawRoll.y), XMConvertToRadians(tData.vPitchYawRoll.x), XMConvertToRadians(tData.vPitchYawRoll.z)
		);

        if (FAILED(CMonster_Base::Create_Mosnter(CBuilder_Map::Change_MakeMonsterType_To_MonsterType(tData.eMakeMonsterType) , iFindPrototypeIndex , iCurLevelIndex  , &tTransformDesc)))
            return E_FAIL;
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

