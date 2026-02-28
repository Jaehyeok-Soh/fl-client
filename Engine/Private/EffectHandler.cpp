#include "Engine_pch.h"
#include "EffectHandler.h"
#include "Model.h"
#include "Bone.h"
#include "Transform.h"
#include "GameObject.h"
#include "PartObject.h"
#include "ContainerObject.h"
#include "MulticastDelegate.h"
#include "EffectBase.h"
#include "DataStruct_EffectEvent.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

CEffectHandler::CEffectHandler() 
    : Super() 
{
}

CEffectHandler::CEffectHandler(const CEffectHandler& rhs)
    : Super(rhs), 
    m_tDesc(rhs.m_tDesc) 
{
}

HRESULT CEffectHandler::Initialize_Prototype(void* pArg)
{
    if(FAILED(Ready_Desc(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectHandler::Initialize(void* pArg) 
{ 
    ANIM_EFFECT_HANDLER_DESC* pDesc = static_cast<ANIM_EFFECT_HANDLER_DESC*>(pArg);

    if (pDesc)
    {
        if (FAILED(Ready_Desc(pArg)))
            return E_FAIL;
    }

    Ready_State();

    return S_OK;
}

HRESULT CEffectHandler::Ready_Desc(void* pArg)
{
    if (pArg)
        m_tDesc = *(static_cast<ANIM_EFFECT_HANDLER_DESC*>(pArg));

    else
        return E_FAIL;

    return S_OK;
}

void CEffectHandler::Set_Desc(const ANIM_EFFECT_HANDLER_DESC& Desc)
{
     m_tDesc = Desc;

     if (m_pOwnerModel)
     {
         auto& vecAnimations = m_pOwnerModel->Get_Animations();

         for (auto& pair : m_tDesc.mapEvents)
         {
             _uint iAnimIndex = pair.first;
             if (iAnimIndex >= vecAnimations.size()) continue;

             auto pAnimation = vecAnimations[iAnimIndex];
             pAnimation->Clear_Notifies();
         }

         Ready_Event();
     }
}
void CEffectHandler::Ready_State()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) return;

    auto iter = m_tDesc.mEffectState.find(E_OBJ_LIFECYCLE_STATE::ON_SPAWN);
    if (iter == m_tDesc.mEffectState.end())
        return;

    m_eCurrentState = E_OBJ_LIFECYCLE_STATE::ON_SPAWN;
}

void CEffectHandler::Awake()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM)
        Ready_AnimState();

    else
        Create_SpawnEffect();
}

void CEffectHandler::Update(_float fDT)
{
}

HRESULT CEffectHandler::Gizmo_Setting()
{

    return S_OK;
}

void CEffectHandler::GetAnimation()
{
    if (Get_Owner())
    {
        m_pOwnerModel = Get_Owner()->Get_Component<CModel>();
        Safe_AddRef(m_pOwnerModel);
    }
}

void CEffectHandler::Ready_Event()
{
    if (m_pOwnerModel == nullptr) return;

    if (m_EventHandle.iID == 0)
    {
        m_EventHandle = m_pOwnerModel->OnNotify.Subscribe([this](const AnimNotifyKey& key) {
            this->CallBackEvent(key);
            });
    }

    auto& vecAnimations = m_pOwnerModel->Get_Animations();

    for (auto& pair : m_tDesc.mapEvents)
    {
        _uint iAnimIndex = pair.first;
        if (iAnimIndex >= vecAnimations.size()) continue;

        auto pAnimation = vecAnimations[iAnimIndex];

        for (size_t i = 0; i < pair.second.size(); ++i)
        {
            auto& tEffectData = pair.second[i];

            _float fTotalTick = pAnimation->Get_DurationTime();
            _float fTickPerSecond = pAnimation->Get_TickPerSecond();
            _float fStartTick = tEffectData.fStartTrackPosition;
            _float fEndTick = fStartTick + (tEffectData.fDuration * fTickPerSecond);


            AnimNotifyKey tOnKey{};
            // fDuration이 있으면 On/Off 관리가 필요한 Attach 타입으로 간주
            tOnKey.eID = (EAnimNotifyId)tEffectData.iNotifyId;
            tOnKey.fTrackPosition = fStartTick;
            tOnKey.iParam0 = (_int)i;
            tOnKey.iParam1 = (_int)pair.first;
            tOnKey.strParam = tEffectData.strEffectTag;

            // Attach 타입이라면 종료 키도 생성
            if (tOnKey.eID == EAnimNotifyId::Vfx_Attach_On)
            {
                AnimNotifyKey tOffKey{};
                tOffKey.eID = EAnimNotifyId::Vfx_Attach_Off;
                tOffKey.fTrackPosition = fEndTick;
                tOffKey.iParam0 = (_int)i;
                tOffKey.iParam1 = (_int)pair.first;
                tOffKey.strParam = tEffectData.strEffectTag;
                pAnimation->Pushback_Notifies(pair.second[i].ePhase, tOffKey);
            }

            pAnimation->Pushback_Notifies(pair.second[i].ePhase, tOnKey);
        }

        pAnimation->Sort_Notifies();
    }
}

HRESULT CEffectHandler::Ready_AnimState()
{
    GetAnimation();

    if (Get_Owner())
    {
        // 툴에서 부모의 Matrix를 참조할 때 안전하게 가져오기
        auto pTransform = Get_Owner()->Get_Component<CTransform>();
        if (pTransform)
        {
            // PartObject 기반이면 부모 Container의 WorldMatrix를 참조함
            auto pPart = static_cast<CPartObject*>(Get_Owner());
            if (pPart->Get_Parent())
                m_pOwnerMatrix = &pPart->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();
            else
                m_pOwnerMatrix = &pTransform->Get_WorldMatrix();
        }
    }

    Ready_Event();

    return S_OK;
}

HRESULT CEffectHandler::Create_SpawnEffect()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) return E_FAIL;

    auto pTransform = Get_Owner()->Get_Component<CTransform>();
    m_pOwnerMatrix = &pTransform->Get_WorldMatrix();

    //if (FAILED(Trigger_Lifecycle_Effect(m_eCurrentState)))
    //    return E_FAIL;

    return S_OK;
}

void CEffectHandler::Release_Event()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM)
    {
        if (m_pOwner)
            m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
    }
}

void CEffectHandler::CallBackEvent(const AnimNotifyKey& key)
{
    if (key.eID == EAnimNotifyId::CollisionOn ||
        key.eID == EAnimNotifyId::CollisionOff ||
        key.eID == EAnimNotifyId::Hitbox)
        return;

    auto iterMap = m_tDesc.mapEvents.find(key.iParam1);
    if (iterMap == m_tDesc.mapEvents.end()) return;

    // 안전한 데이터 접근
    if (key.iParam0 >= iterMap->second.size()) return;
    auto& tEffectData = iterMap->second[key.iParam0];

    // 고유 키 생성 (애니메이션 인덱스와 데이터 순번 조합)
    string strUniqueKey = key.strParam + "_" + std::to_string(key.iParam0);

    switch (key.eID)
    {
    case EAnimNotifyId::Vfx_Oneshot:
        Request_SpawnEffect(tEffectData);
        break;

    case EAnimNotifyId::Vfx_Attach_On:
    {
        Request_SpawnEffect(tEffectData, strUniqueKey);
    }
    break;

    case EAnimNotifyId::Vfx_Attach_Off:
    {
        auto itActive = m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].find(strUniqueKey);

        if (itActive != m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].end())
        {
            m_pGameInstance->Request_DeleteGameObject(
                m_pGameInstance->Get_CurrentLevelIndex(),
                L"Layer_Effect",
                itActive->second
            );
            m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].erase(itActive);
        }
    }
    break;
    }
}

void CEffectHandler::PoolObject_CallBack(CGameObject* pGo)
{
    m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].emplace(pGo->Get_Name(), pGo);
}

unordered_map<_uint, vector<DTO::EFFECTEVENT>>& CEffectHandler::GetEvents()
{
    Ready_Event();

    return m_tDesc.mapEvents;
}

void CEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& Script)
{
    Matrix matTargetWorld = XMMatrixIdentity();

    const Matrix* pTargetBoneMatrix = nullptr;

    if (Script.iBoneIndex != -1 && Script.bFollowBone)
    {
        if (m_pOwnerModel == nullptr)
            return;

        pTargetBoneMatrix = &m_pOwnerModel->Get_Bone(Script.iBoneIndex)->Get_CombinedTransformMatrix();
    }

    Matrix matOwnerNoScale = *m_pOwnerMatrix;

    Vector3 vRight = Vector3(matOwnerNoScale.m[0][0], matOwnerNoScale.m[0][1], matOwnerNoScale.m[0][2]);
    Vector3 vUp = Vector3(matOwnerNoScale.m[1][0], matOwnerNoScale.m[1][1], matOwnerNoScale.m[1][2]);
    Vector3 vLook = Vector3(matOwnerNoScale.m[2][0], matOwnerNoScale.m[2][1], matOwnerNoScale.m[2][2]);

    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();

    matOwnerNoScale.m[0][0] = vRight.x; matOwnerNoScale.m[0][1] = vRight.y; matOwnerNoScale.m[0][2] = vRight.z;
    matOwnerNoScale.m[1][0] = vUp.x;    matOwnerNoScale.m[1][1] = vUp.y;    matOwnerNoScale.m[1][2] = vUp.z;
    matOwnerNoScale.m[2][0] = vLook.x;  matOwnerNoScale.m[2][1] = vLook.y;  matOwnerNoScale.m[2][2] = vLook.z;
    // ------------------------------------------

    matTargetWorld = matOwnerNoScale;

    // 오프셋 적용
    Matrix matOffset = XMMatrixTranslation(Script.vOffset.x, Script.vOffset.y, Script.vOffset.z);
    Matrix matRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Script.vRotation.x), XMConvertToRadians(Script.vRotation.y), XMConvertToRadians(Script.vRotation.z));
   
    matTargetWorld = matOffset * matRotation * matTargetWorld;

    if (Script.iBoneIndex != -1 && Script.bFollowBone)
        matTargetWorld = matOffset * matRotation;

    // 이펙트 생성 요청
    m_pGameInstance->Spawn_PoolEffect(
        Script.strEffectTag,
        matTargetWorld,
        Script.fDuration,
        (_bool)Script.iSimulationType,
        Script.iBoneFlag,
        Script.bFollowBone ? pTargetBoneMatrix : nullptr,
        m_pOwnerMatrix
    );
}

void CEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& script, const std::string& EffectTag)
{
    Matrix matTargetWorld = XMMatrixIdentity();
    Matrix matCustomBone = XMMatrixIdentity();
    Matrix matBoneCopy = XMMatrixIdentity();

    const Matrix* pTargetBoneMatrix = nullptr;

    if (script.iBoneIndex != -1 && script.bFollowBone)
    {
        if (m_pOwnerModel == nullptr)
            return;

        pTargetBoneMatrix = &m_pOwnerModel->Get_Bone(script.iBoneIndex)->Get_CombinedTransformMatrix();
    }


    Matrix matOwnerNoScale = *m_pOwnerMatrix;

    Vector3 vRight = Vector3(matOwnerNoScale.m[0][0], matOwnerNoScale.m[0][1], matOwnerNoScale.m[0][2]);
    Vector3 vUp = Vector3(matOwnerNoScale.m[1][0], matOwnerNoScale.m[1][1], matOwnerNoScale.m[1][2]);
    Vector3 vLook = Vector3(matOwnerNoScale.m[2][0], matOwnerNoScale.m[2][1], matOwnerNoScale.m[2][2]);

    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();

    matOwnerNoScale.m[0][0] = vRight.x; matOwnerNoScale.m[0][1] = vRight.y; matOwnerNoScale.m[0][2] = vRight.z;
    matOwnerNoScale.m[1][0] = vUp.x;    matOwnerNoScale.m[1][1] = vUp.y;    matOwnerNoScale.m[1][2] = vUp.z;
    matOwnerNoScale.m[2][0] = vLook.x;  matOwnerNoScale.m[2][1] = vLook.y;  matOwnerNoScale.m[2][2] = vLook.z;
    // ------------------------------------------

    matTargetWorld = matOwnerNoScale;

    // 오프셋 적용
    Matrix matOffset = XMMatrixTranslation(script.vOffset.x, script.vOffset.y, script.vOffset.z);
    Matrix matRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(script.vRotation.x), XMConvertToRadians(script.vRotation.y), XMConvertToRadians(script.vRotation.z));
    matTargetWorld = matOffset * matRotation * matTargetWorld;

    if (script.iBoneIndex != -1 && script.bFollowBone)
        matTargetWorld = matOffset * matRotation;

    // 이펙트 생성 요청
    m_pGameInstance->Spawn_PoolEffect(
        this,
        EffectTag,
        script.strEffectTag,
        matTargetWorld,
        script.fDuration,
        (_bool)script.iSimulationType,
        script.iBoneFlag,
        script.bFollowBone ? pTargetBoneMatrix : nullptr,
        m_pOwnerMatrix
    );
}

HRESULT CEffectHandler::Trigger_Lifecycle_Effect(E_OBJ_LIFECYCLE_STATE eState)
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) 
        return E_FAIL;

    if (m_ePrevState == eState) return E_FAIL;

    CGameObject* pGo = nullptr;

    // 이전 State 꺼내오기. (들어온 State가 Spawn이면 안된다.)
    if (eState != E_OBJ_LIFECYCLE_STATE::ON_SPAWN)
    {
        pGo = m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)][(m_tDesc.mEffectState[m_ePrevState].EffectPrefabTag)];

        if (pGo == nullptr)
        {
            MSG_BOX("추가 되지 않은 State거나 저장되지 않은 Object입니다");
            return E_FAIL;
        }
        CEffectBase* pBase = static_cast<CEffectBase*>(pGo);
        if (pBase)
        {
            // 이전 State Loop 종료시키기 (Dissolve 되게끔.)
            pBase->LoopStateChange(DTO::E_LoopState::LOOP_END);
        }
    }

    // State가 등록되어있는지 확인절차를 한다. 만약에 없으면 Event를 통해서 레이어 추가를 시도하면 안되니까.
    auto iter = m_tDesc.mEffectState.find(eState);
    if (iter == m_tDesc.mEffectState.end() || iter->second.EffectPrefabTag.empty())
    {
        // 등록된 이펙트가 없으면 그냥 상태만 바꾸고 정상 종료
        m_ePrevState = m_eCurrentState = eState;
        return S_OK;
    }

    // 현재 들어온 State에 맞는 이펙트 꺼내오기.
    STATE_VFX_DESC pDesc = m_tDesc.mEffectState[eState];

    DTO::EFFECTEVENT pEvent = {};
    pEvent.strEffectTag = pDesc.EffectPrefabTag;
    pEvent.bFollowBone = pDesc.bLocal;
    pEvent.iBoneIndex = pDesc.iBoneIndex;
    pEvent.iSimulationType = pDesc.bLocal;
    pEvent.vOffset = pDesc.vOffSet;
    pEvent.vRotation = pDesc.vRotation;

    Request_SpawnEffect(pEvent, pDesc.EffectPrefabTag);

    // State 변경
    m_ePrevState = m_eCurrentState = eState;

    return S_OK;
}

CEffectHandler* CEffectHandler::Create(void* pArg)
{
    CEffectHandler* pInstance = new CEffectHandler();
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Created : CEffectHandler");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CEffectHandler::Clone(void* pArg)
{
    CEffectHandler* pInstance = new CEffectHandler(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffectHandler");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffectHandler::Free()
{
    Release_Event();

    for (_uint i = 0; i < ENUM_TO_UINT(E_HANDLER_TYPE::TYPE_END); ++i)
    {
        m_ActiveEffects[i].clear();
    }

    // 3. 캐싱된 모델 참조 해제
    Safe_Release(m_pOwnerModel);

    Super::Free();
}