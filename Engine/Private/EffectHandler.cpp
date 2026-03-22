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
             wstring iAnimName = Engine_Utils::ToWString(pair.first);

             for (auto& pAnimation : vecAnimations)
             {
                 if (pAnimation->Get_Name() == iAnimName)
                 {
                     pAnimation->Clear_Notifies(EAnimNotifyId::Vfx_Oneshot);
                 }
             }
         }
         Ready_AnimState();
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

void CEffectHandler::Setup_ForOwner(CGameObject* pOwner, CModel* pModel)
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM)
    {
        if (FAILED(Owner_Setting(pOwner)))
        {
            MSG_BOX("Owner Setting Fail : EffectHandler");
            return;
        }

        Setup_OwnerModel(pModel);
        Ready_AnimState();
    }

    else
    {
        if (FAILED(Owner_Setting(pOwner)))
        {
            MSG_BOX("Owner Setting Fail : EffectHandler");
            return;
        }

        Create_SpawnEffect();
    }

}

void CEffectHandler::Setup_OwnerModel(CModel* pModel)
{
    if (Get_Owner())
    {
        if (m_pOwnerModel == nullptr)
        {
            m_pOwnerModel = pModel;
            Safe_AddRef(m_pOwnerModel);
        }
    }
}


void CEffectHandler::Update(_float fDT)
{
}

HRESULT CEffectHandler::Gizmo_Setting()
{

    return S_OK;
}


HRESULT CEffectHandler::Ready_AnimState()
{
    if (m_pOwnerModel == nullptr) return E_FAIL;

    if (m_EventHandle.iID == 0)
    {
        m_EventHandle = m_pOwnerModel->OnNotify.Subscribe([this](const AnimNotifyKey& key) {
            this->CallBackEvent(key);
            });
    }

    auto& vecAnimations = m_pOwnerModel->Get_Animations();

    for (auto& pair : m_tDesc.mapEvents)
    {
        wstring iAnimName = Engine_Utils::ToWString(pair.first);

        for (auto& pAnimation : vecAnimations)
        {
            if (pAnimation->Get_Name() == iAnimName)
            {
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
                    tOnKey.strParam = tEffectData.strEffectTag;
                    tOnKey.strParam2 = pair.first;

                    // Attach 타입이라면 종료 키도 생성
                    if (tOnKey.eID == EAnimNotifyId::Vfx_Attach_On)
                    {
                        AnimNotifyKey tOffKey{};
                        tOffKey.eID = EAnimNotifyId::Vfx_Attach_Off;
                        tOffKey.fTrackPosition = fEndTick;
                        tOffKey.iParam0 = (_int)i;
                        tOffKey.strParam = tEffectData.strEffectTag;
                        tOffKey.strParam2 = pair.first;
                        pAnimation->Pushback_Notifies(pair.second[i].ePhase, tOffKey);
                    }

                    pAnimation->Pushback_Notifies(pair.second[i].ePhase, tOnKey);
                }

                pAnimation->Sort_Notifies();
            }
        }  
    }

    return S_OK;
}

HRESULT CEffectHandler::Create_SpawnEffect()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) return E_FAIL;

    m_pOwnerMatrix = m_tDesc.mEffectState[E_OBJ_LIFECYCLE_STATE::ON_SPAWN].pParentTransformMatrix;

    return S_OK;
}

void CEffectHandler::Compute_LocalRotation(TRANSFORM_INFO_STATE eRotateState, _float fDegree, OUT Matrix& outMatLocalRotation)
{
    switch (eRotateState)
    {
    case Engine::TRANSFORM_INFO_STATE::RIGHT:
        outMatLocalRotation = Matrix::CreateFromAxisAngle(Vec3::Right, ::XMConvertToRadians(fDegree));
        break;
    case Engine::TRANSFORM_INFO_STATE::UP:
        outMatLocalRotation = Matrix::CreateFromAxisAngle(Vec3::Up, ::XMConvertToRadians(fDegree));
        break;
    case Engine::TRANSFORM_INFO_STATE::LOOK:
        outMatLocalRotation = Matrix::CreateFromAxisAngle(Vec3::Backward, ::XMConvertToRadians(fDegree));
        break;
    default:
        outMatLocalRotation = Matrix::Identity;
        break;
    }
}

void CEffectHandler::Release_Event()
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM)
    {
        if (m_pOwner)
            if(m_pOwnerModel)
                m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
    }
}

void CEffectHandler::CallBackEvent(const AnimNotifyKey& key)
{
    if (key.eID == EAnimNotifyId::CollisionOn ||
        key.eID == EAnimNotifyId::CollisionOff ||
        key.eID == EAnimNotifyId::Hitbox)
        return;

    // 애니메이션 string 데이터 
    auto iterMap = m_tDesc.mapEvents.find(key.strParam2);
    if (iterMap == m_tDesc.mapEvents.end()) return;

    // 안전한 데이터 접근
    if (key.iParam0 >= iterMap->second.size()) return;
    auto& tEffectData = iterMap->second[key.iParam0];
    tEffectData.iAnimIndex = key.iParam0;

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
                itActive->second
            );
            m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].erase(itActive);
        }
    }
    break;
    }
}

HRESULT CEffectHandler::Owner_Setting(CGameObject* pGo)
{
    // 넣어준 값이 없다면 여기서 설정을 해준다.
    if (m_pOwnerMatrix == nullptr)
    {
        if (pGo)
            m_pOwnerMatrix = &pGo->Get_Component<CTransform>()->Get_WorldMatrix();
    }

    return m_pOwnerMatrix ? S_OK : E_FAIL;
}

void CEffectHandler::PoolObject_CallBack(CGameObject* pGo)
{
    // 콜백 받아온 ActiveEffect를 관리 항목으로 저장한다.
    m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)].emplace(pGo->Get_Name(), pGo);
}

void CEffectHandler::Reset_ForNextLevel()
{
    m_eCurrentState = E_OBJ_LIFECYCLE_STATE::NONE;
    m_ePrevState = E_OBJ_LIFECYCLE_STATE::NONE;

    for (_uint i = 0; i < ENUM_TO_UINT(E_HANDLER_TYPE::TYPE_END); ++i)
    {
        m_ActiveEffects[i].clear();
    }
}

void CEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& script)
{
    Matrix MatWorldOffset = XMMatrixIdentity();
    Matrix MatOwnerMatrix = XMMatrixIdentity();
    const Matrix* pTargetBoneMatrix = nullptr;

    // 뼈 행렬 계산
    BoneMatrix_CalCulator(script, pTargetBoneMatrix);
    // 애니메이션 모델 떄문에 Scale 행렬을 날려주는 값을 넣어준다.
    MatOwnerMatrix = Delete_ScaleMatrix(*m_pOwnerMatrix);
    // 오프셋 적용
    MatWorldOffset = Offset_CalCulator(script);
    // 이펙트 생성 요청
    Spawn_RequestFromEffectManager(script, MatWorldOffset, MatOwnerMatrix, pTargetBoneMatrix);
    //Spawn_RequestFromEffectManager(script, MatWorldOffset, pTargetBoneMatrix, MatWorldOffset);
}

void CEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& script, const std::string& EffectTag)
{
    Matrix MatWorldOffset = XMMatrixIdentity();
    Matrix MatOwnerMatrix = XMMatrixIdentity();
    const Matrix* pTargetBoneMatrix = nullptr;

    // 뼈 행렬 계산
    BoneMatrix_CalCulator(script, pTargetBoneMatrix);
    // 애니메이션 모델 떄문에 Scale 행렬을 날려주는 값을 넣어준다.
    MatOwnerMatrix = Delete_ScaleMatrix(*m_pOwnerMatrix);
    // 오프셋 적용
    MatWorldOffset = Offset_CalCulator(script);

    Spawn_RequestFromEffectManager(script, MatWorldOffset, MatOwnerMatrix, pTargetBoneMatrix, EffectTag);
}

void CEffectHandler::Request_SpawnEffect(const DTO::EFFECTEVENT& script, const std::string& EffectTag, TRANSFORM_INFO_STATE eRotateState, _float fDegree)
{
    Matrix MatWorldOffset = Matrix::Identity;
    Matrix MatOwnerMatrix = Matrix::Identity;
    Matrix MatLocalRotationMatrix = Matrix::Identity;
    Compute_LocalRotation(eRotateState, fDegree, MatLocalRotationMatrix);
    const Matrix* pTargetBoneMatrix = nullptr;

    // 뼈 행렬 계산
    BoneMatrix_CalCulator(script, pTargetBoneMatrix);
    // 애니메이션 모델 떄문에 Scale 행렬을 날려주는 값을 넣어준다.
    MatOwnerMatrix = Delete_ScaleMatrix(*m_pOwnerMatrix);
    // 오프셋 적용
    MatWorldOffset = Offset_CalCulator(script);
    MatWorldOffset = MatLocalRotationMatrix * MatWorldOffset;
    // 이펙트 생성 요청
    Spawn_RequestFromEffectManager(script, MatWorldOffset, MatOwnerMatrix, pTargetBoneMatrix, EffectTag);
}

HRESULT CEffectHandler::Trigger_Lifecycle_Effect(E_OBJ_LIFECYCLE_STATE eState)
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) return E_FAIL;
    if (m_ePrevState == eState) return S_OK;

    auto& activeMap = m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)];
    string prevTag = m_tDesc.mEffectState[m_ePrevState].EffectPrefabTag;

    auto it = activeMap.find(prevTag);
    if (it != activeMap.end())
    {
        // TODO 
        // 풀에 돌아간 EFfect인지 체크를 함.
        CEffectBase* pBase = static_cast<CEffectBase*>(it->second);
        if (pBase && !pBase->IsPooled())
            pBase->LoopStateChange(DTO::E_LoopState::LOOP_END);

        activeMap.erase(it);
    }

    // State가 등록되어있는지 확인절차를 한다. 만약에 없으면 Event를 통해서 레이어 추가를 시도하면 안되니까.
    auto iter = m_tDesc.mEffectState.find(eState);
    if (iter == m_tDesc.mEffectState.end() || iter->second.EffectPrefabTag.empty())
    {
        // 등록된 이펙트가 없으면 그냥 상태만 바꾸고 정상 종료
        m_ePrevState = m_eCurrentState = eState;
        return S_OK;
    }

    // 스폰 요청.
    Request_SpawnEffect(Write_EffectEventDesc(eState), m_tDesc.mEffectState[eState].EffectPrefabTag);

    // State 변경
    m_ePrevState = m_eCurrentState = eState;

    return S_OK;
}

HRESULT CEffectHandler::Trigger_Lifecycle_Effect(E_OBJ_LIFECYCLE_STATE eState, TRANSFORM_INFO_STATE eRotateState, _float fDegree)
{
    if (m_tDesc.eType == E_HANDLER_TYPE::MODEL_ANIM) return E_FAIL;
    if (m_ePrevState == eState) return S_OK;
#ifdef _DEBUG
    switch (eRotateState)
    {
    case Engine::TRANSFORM_INFO_STATE::POS:
    case Engine::TRANSFORM_INFO_STATE::END:
        MSG_BOX("CEffectHandler::Trigger_Lifecycle_Effect, eRotateState is POS, END");
        return E_FAIL;
    }
#endif

    auto& activeMap = m_ActiveEffects[ENUM_TO_UINT(m_tDesc.eType)];
    string prevTag = m_tDesc.mEffectState[m_ePrevState].EffectPrefabTag;

    auto it = activeMap.find(prevTag);
    if (it != activeMap.end())
    {
        // TODO 
        // 풀에 돌아간 EFfect인지 체크를 함.
        CEffectBase* pBase = static_cast<CEffectBase*>(it->second);
        if (pBase && !pBase->IsPooled())
            pBase->LoopStateChange(DTO::E_LoopState::LOOP_END);

        activeMap.erase(it);
    }

    // State가 등록되어있는지 확인절차를 한다. 만약에 없으면 Event를 통해서 레이어 추가를 시도하면 안되니까.
    auto iter = m_tDesc.mEffectState.find(eState);
    if (iter == m_tDesc.mEffectState.end() || iter->second.EffectPrefabTag.empty())
    {
        // 등록된 이펙트가 없으면 그냥 상태만 바꾸고 정상 종료
        m_ePrevState = m_eCurrentState = eState;
        return S_OK;
    }

    // 스폰 요청.
    Request_SpawnEffect(Write_EffectEventDesc(eState), m_tDesc.mEffectState[eState].EffectPrefabTag, eRotateState, fDegree);

    // State 변경
    m_ePrevState = m_eCurrentState = eState;

    return S_OK;
}

void CEffectHandler::Spawn_RequestFromEffectManager(
    const DTO::EFFECTEVENT& script,
    SimpleMath::Matrix& OffsetMat,
    SimpleMath::Matrix& OwnerMatrix,
    const Matrix* BoneMatrix,
    const std::string& EffectTag)
{
    Matrix matTargetWorld = XMMatrixIdentity();

    if (script.iSimulationType == ENUM_TO_UINT(E_WORLD::E_LOCAL))
    {
        matTargetWorld = OffsetMat;
    }

    else if (script.iSimulationType == ENUM_TO_UINT(E_WORLD::E_WORLD))
    {
        matTargetWorld = OffsetMat * OwnerMatrix;
    }


    EFFECT_SPAWN_DESC tEngineDesc = {};
    tEngineDesc.matWorld = matTargetWorld;
    tEngineDesc.iSimulationType = (_bool)script.iSimulationType;
    tEngineDesc.pTargetBoneMatrix = ((script.bFollowBone || script.bUseChildBone) ? &BoneMatrix : nullptr);
    tEngineDesc.pTransformMatrix = &m_pOwnerMatrix;
    tEngineDesc.iBoneFlag = script.iBoneFlag;

    if (script.iChildBoneIndex != -1 && script.bUseChildBone)
    {
        tEngineDesc.bUseChildBone = true;
    }

    //if(m_pOwnerModel)
    //tEngineDesc.VFX_fSpeed = m_pOwnerModel->Get_Animatioin_MotionOffset(script.iAnimIndex);

    // 이펙트 생성 요청
    m_pGameInstance->Request_Effect(this, EffectTag, script.strEffectTag, tEngineDesc);
}

void CEffectHandler::Spawn_RequestFromEffectManager(
    const DTO::EFFECTEVENT& script,
    SimpleMath::Matrix& OffsetMat,
    SimpleMath::Matrix& OwnerMatrix,
    const Matrix* BoneMatrix)
{
    Matrix matTargetWorld = XMMatrixIdentity();

    if (script.iSimulationType == ENUM_TO_UINT(E_WORLD::E_LOCAL))
    {
        matTargetWorld = OffsetMat;
    }

    else if (script.iSimulationType == ENUM_TO_UINT(E_WORLD::E_WORLD))
    {
        matTargetWorld = OffsetMat * OwnerMatrix;
    }

    EFFECT_SPAWN_DESC tEngineDesc = {};
    tEngineDesc.matWorld = matTargetWorld;
    tEngineDesc.iSimulationType = (_bool)script.iSimulationType;
    tEngineDesc.pTargetBoneMatrix = ((script.bFollowBone || script.bUseChildBone) ? &BoneMatrix : nullptr);
    tEngineDesc.pTransformMatrix = &m_pOwnerMatrix;
    tEngineDesc.iBoneFlag = script.iBoneFlag;

    if (script.iChildBoneIndex != -1 && script.bUseChildBone)
    {
        tEngineDesc.bUseChildBone = true;
    }

 /*   if (m_pOwnerModel)
    tEngineDesc.VFX_fSpeed = m_pOwnerModel->Get_Animatioin_MotionOffset(script.iAnimIndex);*/
  
    // 이펙트 생성 요청
    m_pGameInstance->Request_Effect(script.strEffectTag, tEngineDesc);
}

DTO::EFFECTEVENT CEffectHandler::Write_EffectEventDesc(const E_OBJ_LIFECYCLE_STATE eState)
{
    // 현재 들어온 State에 맞는 이펙트 꺼내오기.
    STATE_VFX_DESC pDesc = m_tDesc.mEffectState[eState];

    DTO::EFFECTEVENT pEvent = {};
    pEvent.strEffectTag = pDesc.EffectPrefabTag;
    pEvent.bFollowBone = pDesc.bFollowBone;
    pEvent.iBoneIndex = pDesc.iBoneIndex;
    pEvent.iSimulationType = ENUM_TO_UINT(pDesc.bWorld);
    pEvent.vOffset = pDesc.vOffSet;
    pEvent.vRotation = pDesc.vRotation;

    return pEvent;
}

void CEffectHandler::BoneMatrix_CalCulator(const DTO::EFFECTEVENT& script, OUT const SimpleMath::Matrix*& BoneMatrix)
{
    if (script.iChildBoneIndex != -1 && script.bUseChildBone)
    {
        if (Get_Owner() == nullptr)
            return;

        if (dynamic_cast<CContainerObject*>(Get_Owner()))
        {
            // 플레이어 총 무기
            if (script.ChildPartNumber == -1) return;

            auto pWeapon = static_cast<CContainerObject*>(Get_Owner())->Get_Part<CPartObject>(script.ChildPartNumber);
            if (pWeapon == nullptr)
                return;

            CModel* pWeaponModel = pWeapon->Get_Component<CModel>();
            if (pWeaponModel == nullptr)
                return;
             
            (BoneMatrix) = &pWeaponModel->Get_Bone(script.iChildBoneIndex)->Get_CombinedTransformMatrix();
        }
    }
    else
    {
        if (script.iBoneIndex != -1 && script.bFollowBone)
        {
            if (m_pOwnerModel == nullptr)
                return;

            (BoneMatrix) = &m_pOwnerModel->Get_Bone(script.iBoneIndex)->Get_CombinedTransformMatrix();
        }
    }
}

SimpleMath::Matrix CEffectHandler::Offset_CalCulator(const DTO::EFFECTEVENT& script)
{
    // 오프셋 적용
    Matrix matOffset = XMMatrixTranslation(script.vOffset.x, script.vOffset.y, script.vOffset.z);
    Matrix matRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(script.vRotation.x), XMConvertToRadians(script.vRotation.y), XMConvertToRadians(script.vRotation.z));

    Matrix ResultMatrix = {};

    ResultMatrix = (matOffset * matRotation);

    if (script.iChildBoneIndex != -1 && script.bUseChildBone)
    {
        if (Get_Owner() == nullptr)
            return ResultMatrix;

        if (dynamic_cast<CContainerObject*>(Get_Owner()))
        {
            // 플레이어 총 무기
            if (script.ChildPartNumber == -1) return ResultMatrix;

            auto pWeapon = static_cast<CContainerObject*>(Get_Owner())->Get_Part<CPartObject>(script.ChildPartNumber);
            if (pWeapon == nullptr)
                return ResultMatrix;

            Matrix WeaponCombinedMatrix = pWeapon->Get_CombinedMatrix();
            Matrix matCustom2 = XMMatrixIdentity();

            Vector3 vBoneScale2;
            Quat vBoneQuat2;
            Vector3 vBonePos2;

            WeaponCombinedMatrix.Decompose(vBoneScale2,d vBoneQuat2, vBonePos2);
            matCustom2 *= Matrix::CreateFromQuaternion(vBoneQuat2);
            matCustom2.Translation(Vec3(vBonePos2.x, vBonePos2.y, vBonePos2.z));
            (ResultMatrix) *= matCustom2;
        }
    }

    return ResultMatrix;
}

SimpleMath::Matrix CEffectHandler::Delete_ScaleMatrix(SimpleMath::Matrix Mat)
{
    Matrix matOwnerNoScale = Mat;

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

    return matOwnerNoScale;
}

// 툴용
unordered_map<string, vector<DTO::EFFECTEVENT>>& CEffectHandler::GetEvents()
{
    Ready_AnimState();

    return m_tDesc.mapEvents;
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